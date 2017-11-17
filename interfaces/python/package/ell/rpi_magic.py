import tempfile
import os
import paramiko
from IPython.core.magic import Magics, magics_class, cell_magic, line_magic
from IPython.core.display import display
from ipywidgets import Button, HBox, HTML, Label, Output, Layout
from .util.commands import run


@magics_class
class RaspberryPi(Magics):
    remote_pid = None
    status_label = Label(
        value='', layout=Layout(width='500px', max_width='500px'))
    client = None
    password = None

    def save_cell(self, cell, fname):
        dir = tempfile.mkdtemp()
        path = os.path.join(dir, fname)
        with open(path, 'wb') as fp:
            fp.write(bytes(cell, 'UTF-8'))
        return path

    def feedback(self, msg):
        self.status_label.value = msg

    def open_remote(self, opts):
        self.client = paramiko.SSHClient()
        self.client.load_system_host_keys()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.client.connect(
            opts['ip'], username=opts['user'], password=self.password)

    def remote_copy(self, files, rpi_path):
        with self.client.open_sftp() as sftp:
            for file in files:
                self.feedback('sftp ' + file)
                lastpct = 0

                def report_progress(sofar, total):
                    nonlocal lastpct, file
                    pct = sofar / total * 100
                    delta = pct - lastpct
                    if delta > 10 and delta < 90:
                        self.feedback('sftp ' + file + " {:.0f}%".format(pct))
                        lastpct = pct

                local_path = rpi_path + '/' + os.path.basename(file)
                # I don't like this logic :(
                if (file.find('/include/') >= 0):
                    local_path = rpi_path + '/include/' + os.path.basename(file)
                if (file.find('/tcc/') >= 0):
                    local_path = rpi_path + '/tcc/' + os.path.basename(file)
                sftp.put(file, local_path, callback=report_progress)

    def copy_model_to_rpi(self, model, rpi_path):
        pkgdir = os.path.dirname(__file__)
        files = model.files() + [
            pkgdir + '/deploy/OpenBLASSetup.cmake',
            pkgdir + '/deploy/include/CallbackInterface.h',
            pkgdir + '/deploy/tcc/CallbackInterface.tcc',
            pkgdir + '/vision/demoHelper.py',
        ]

        # Compare the mod times, to see if the remote copy is up to date
        test_file = files[0]
        remote_getmtime = 'python -c "import os.path; print(os.path.getmtime(\'{}\'))"'.format(
            rpi_path + '/' + os.path.basename(test_file))
        remote_mtime_string = self.remote_eval(remote_getmtime)
        if remote_mtime_string:
            remote_mtime = float(remote_mtime_string)
            local_mtime = os.path.getmtime(test_file)
            if local_mtime < remote_mtime:
                # local file is older, so skip copying and building model
                return

        self.remote_command('rm -r -f ' + rpi_path + '; mkdir -p ' + rpi_path +
                            '/include; mkdir -p ' + rpi_path + '/tcc')
        self.remote_copy(files, rpi_path)
        self.feedback('Building...')
        self.remote_command('cd ' + rpi_path +
                            '; mkdir build; cd build; cmake ..; make')

    def remote_eval(self, command):
        _, stdout, _ = self.client.exec_command(command, bufsize=0)
        result = ''
        for line in stdout:
            result += line.strip('\n')
        return result

    def remote_command(self, command):
        _, stdout, stderr = self.client.exec_command(command, bufsize=0)
        for line in stdout:
            print(line.strip('\n'))
        for line in stderr:
            print(line.strip('\n'))

    def remote_job(self, command):
        command = 'echo $$; ' + command
        _, stdout, stderr = self.client.exec_command(command, bufsize=0)
        pid = stdout.readline().strip('\n')
        self.feedback('PID ' + pid)

        def report_output(stdout, stderr):
            try:
                import sys
                for line in stdout:
                    print(line.strip('\n'))
                    sys.stdout.flush()
                for line in stderr:
                    print(line.strip('\n'))
                    sys.stdout.flush()
                self.remote_pid = None
            except Exception as e:
                print(e)
                import traceback
                traceback.print_exc()
                sys.stdout.flush()

        from IPython.lib import backgroundjobs as bg
        jobs = bg.BackgroundJobManager()
        # The new() method does an unfortunate print statement.
        # So we switch the printing context to a detached Output to throw it away.
        tossout = Output()
        with tossout:
            jobs.new(report_output, stdout, stderr)
        return pid

    @cell_magic
    def rpi(self, line, cell):
        'provide a user interface for remotely executing code on the RPi'
        opts, _ = self.parse_options(line, '', 'user=', 'ip=', 'model=',
                                     'rpipath=')
        rpi_path = opts['rpipath']
        model = get_ipython().ev(opts['model'])

        self.password = 'intelligentDevices'  # TEMPORARY FOR DEBUGGING!
        if not self.password:
            import getpass
            self.password = getpass.getpass(
                prompt='Password on the Raspberry Pi ')

        def stop_process(b):
            if self.remote_pid:
                self.feedback('kill ' + self.remote_pid)
                # The second minus sign below kills the whole process group (bash + python).
                self.remote_command("kill -KILL -" + self.remote_pid)

        stop_button = Button(description='Stop running')
        stop_button.on_click(stop_process)

        display(HBox([stop_button, self.status_label]))

        try:
            self.feedback('Copying model')
            self.open_remote(opts)
            self.copy_model_to_rpi(model, rpi_path)
            actuationpy = self.save_cell(cell, 'actuation.py')
            self.remote_copy([actuationpy], rpi_path)

            self.status_label.value = 'Running'
            self.remote_pid = self.remote_job(
                'cd ' + rpi_path + '; ' +
                '/home/pi/miniconda3/bin/python3 actuation.py')
        except paramiko.AuthenticationException:
            self.feedback(
                'Authentication failed. Wrong password? Evaluate the cell to try again'
            )
            self.password = None
        except TimeoutError:
            self.feedback('Timeout. Wrong IP address?')


def init_magics():
    try:
        ipy = get_ipython()
        # ipy.register_magics(Arduino)
        ipy.register_magics(RaspberryPi)
    except NameError:
        pass  # We're in regular Python, not Jupyter, so we can't use magics
