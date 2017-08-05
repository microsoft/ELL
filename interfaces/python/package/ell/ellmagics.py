from IPython.core.magic import Magics, magics_class, cell_magic, line_magic
import tempfile
import os
from .util.commands import run, print as feedback
import paramiko


_arduino_exe_dir = 'C:\\Program Files (x86)\\Arduino'
_arduino_exe = 'arduino_debug.exe'

class _CommandMagic(Magics):

    def save_cell(self, cell, fname):
        dir = tempfile.mkdtemp()
        path = os.path.join(dir, fname)
        with open(path, 'wb') as fp:
            fp.write(bytes(cell, 'UTF-8'))
        return path


@magics_class
class Arduino(_CommandMagic):

    @cell_magic
    def arduino(self, line, cell):
        "compile and upload cell to arduino"

        # Create a temporary sketch file with the cell's contents.
        fname = self.save_cell(cell, 'actuation.ino')

        # Run the arduino executable to compile and upload.
        os.chdir(_arduino_exe_dir)

        opts, _ = self.parse_options(line, 'p:')

        def pre(s):
            return '<pre>' + s + '</pre>'

        display(HTML(pre('Compiling...')))
        cmd = [_arduino_exe]
        if 'port' in opts:
            cmd += ['--upload', '--port', opts.port]
        else:
            cmd += ['--verify']
        cmd += [fname]

        run(cmd)


@magics_class
class RaspberryPi(_CommandMagic):
    password = None

    def open_remote(self):
        self.client = paramiko.SSHClient()
        self.client.load_system_host_keys()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.client.connect(self.ip, username=self.user, password=self.password)

    def remote_command(self, command):
        feedback(command)
        stdin, stdout, stderr = self.client.exec_command(command, bufsize=0)
        for line in stdout:
            feedback(line.strip('\n'))
        for line in stderr:
            feedback(line.strip('\n'))

    def remote_copy(self, files, pipath):
        with self.client.open_sftp() as sftp:
            for file in files:
                feedback('sftp ' + file)
                lastpct = 0
                def progress(sofar, total):
                    nonlocal lastpct
                    pct = sofar / total * 100
                    delta = pct - lastpct
                    if delta > 10 and delta < 90:
                        feedback("{:.0f}%".format(pct))
                        lastpct = pct
                sftp.put(file, pipath + '/' + os.path.basename(file), callback=progress)

    @line_magic
    def rpi_deploy(self, line):
        opts, args = self.parse_options(line, '', 'user=', 'ip=', 'path=', 'pipath=')

        if not self.password:
            import getpass
            self.password = getpass.getpass(prompt='Password on the Raspberry Pi ')

        try:
            self.user = opts['user']
            self.ip = opts['ip']
            self.path = opts['path']
            self.path += '/model'
            self.pipath = opts['pipath']
            dir = os.path.basename(self.path)

            self.open_remote()

            feedback('Copying files...')
            self.remote_command('rm -r -f ' + self.pipath)
            self.remote_command('mkdir -p ' + self.pipath)
            pkgdir = os.path.dirname(__file__)
            files = [
                # build files
                pkgdir + '/Release/deploy/CMakeLists.txt',
                pkgdir + '/Release/deploy/OpenBLASSetup.cmake',
                # python code
                pkgdir + '/vision/modelHelper.py',
                self.path + '.py',
                # native code
                self.path + 'PYTHON_wrap.cxx',
                self.path + 'PYTHON_wrap.h',
                pkgdir + '/Release/deploy/darknetImageNetLabels.txt',
                pkgdir + '/Release/deploy/CallbackInterface.h',
                pkgdir + '/Release/deploy/ClockInterface.h',
                pkgdir + '/Release/deploy/CallbackInterface.tcc',
                self.path + '.i.h',
                self.path + '.o',
            ]
            self.remote_copy(files, self.pipath)
            feedback('Building...')
            self.remote_command('cd ' + self.pipath + '; mkdir build; cd build; cmake ..; make')

            feedback('Done')
            self.client.close()
        except paramiko.AuthenticationException:
            feedback('Authentication failed. Wrong password? Evaluate the cell to try again')
            self.password = None


    @cell_magic
    def rpi_run(self, line, cell):
        self.open_remote()

        actuationpy = self.save_cell(cell, 'actuation.py')
        self.remote_copy([actuationpy], self.pipath)

        self.remote_command('cd ' + self.pipath + '; /home/pi/miniconda3/bin/python3 actuation.py')





def init_magics():
    try:
        ip = get_ipython()
        ip.register_magics(Arduino)
        ip.register_magics(RaspberryPi)
    except NameError:
        pass