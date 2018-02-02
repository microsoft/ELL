"""
A Jupyter magic to allow code to be copied and deployed on the Raspberry Pi directly 
from the notebook.
"""
import tempfile
import os
import sys
import paramiko

from . import platform
from IPython import get_ipython
from ipywidgets import Button, HBox, Label, Output, Layout
from IPython.core.magic import Magics, magics_class, cell_magic

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
        files = model.files(platform.PI3) + [
            pkgdir + '/deploy/OpenBLASSetup.cmake',
            pkgdir + '/deploy/include/CallbackInterface.h',
            pkgdir + '/deploy/tcc/CallbackInterface.tcc',
            pkgdir + '/util/tutorialHelpers.py',
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

    def print_output(self, line):
        line = line.strip('\n')
        print(line)            
        sys.stdout.flush()

    def remote_command(self, command):
        command = 'echo $$; ' + command
        _, stdout, stderr = self.client.exec_command(command, bufsize=0)
        self.remote_pid = stdout.readline().strip('\n')
        for line in stdout:
            self.print_output(line)
        for line in stderr:
            self.print_output(line)

    def remote_command_with_callback(self, command, callback):
        try:
            self.remote_command(command)
        except Exception as e:
            print("### Remote command failed.")
            print(e)
            import traceback
            traceback.print_exc()
            sys.stdout.flush()
        callback()

    def remote_command_async(self, command, callback):
        from IPython.lib import backgroundjobs as bg
        jobs = bg.BackgroundJobManager()
        jobs.new(self.remote_command_with_callback, command, callback)

    @cell_magic
    def rpi(self, line, cell):
                
        from IPython.core.display import display

        'provide a user interface for remotely executing code on the RPi'
        opts, _ = self.parse_options(line, '', 'user=', 'ip=', 'model=',
                                     'rpipath=', 'password=')
        rpi_path = opts['rpipath']
        model = get_ipython().ev(opts['model'])
        if not self.password and 'password' in opts:
            self.password = opts['password']

        if not self.password:
            import getpass
            self.password = getpass.getpass(
                prompt='Password on the Raspberry Pi ')

        def stop_process(b):
            if self.remote_pid:
                self.feedback('kill ' + self.remote_pid)
                # The second minus sign below kills the whole process group (bash + python).
                self.remote_command("kill -KILL -" + self.remote_pid)

        stop_button = Button(description='Stop running', disabled=False)
        stop_button.on_click(stop_process)

        display(HBox([stop_button, self.status_label]))

        try:
            self.feedback('Copying model')
            self.open_remote(opts)
            self.copy_model_to_rpi(model, rpi_path)
            actuationpy = self.save_cell(cell, 'actuation.py')
            self.remote_copy([actuationpy], rpi_path)

            self.status_label.value = 'Running'
            self.remote_command_async(
                'cd ' + rpi_path + '; ' +
                'source /home/pi/miniconda3/envs/py34/bin/activate py34 > /dev/null 2>&1; ' + 
                'echo running remote python script...; ' +
                'python3 actuation.py', 
                lambda:self.on_job_complete(stop_button))
        except paramiko.AuthenticationException:
            self.feedback(
                'Authentication failed. Wrong password? Evaluate the cell to try again.'
            )
            self.password = None
        except TimeoutError:
            self.feedback('Timeout while trying to reach the Raspberry Pi. Wrong IP address?')
        except:
            errorType, value, traceback = sys.exc_info()
            self.feedback("### Exception: " + str(errorType) + ": " + str(value))

    def on_job_complete(self, stop_button):
        stop_button.description = "Completed"
        stop_button.disabled = True

def init_magics():
    try:
        ipy = get_ipython()
        if ipy:
            op = getattr(ipy, "register_magics", None)
            if callable(op):
                ipy.register_magics(RaspberryPi)
                platform.has_magic = True
    except:
        pass  # We're in regular Python, not Jupyter, so we can't use magics
