from IPython.core.magic import Magics, magics_class, cell_magic
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

    @cell_magic
    def raspberry_pi(self, line, cell):
        opts, args = self.parse_options(line, '', 'user=', 'ip=', 'path=', 'pipath=')

        if not self.password:
            import getpass
            self.password = getpass.getpass(prompt='Password on the Raspberry Pi ')

        user = opts['user']
        ip = opts['ip']
        path = opts['path']
        path += '/model'
        pipath = opts['pipath']
        dir = os.path.basename(path)
        actuationpy = self.save_cell(cell, 'actuation.py')

        client = paramiko.SSHClient()
        client.load_system_host_keys()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect(ip, username=user, password=self.password)

        def remote_command(command):
            nonlocal client
            feedback(command)
            stdin, stdout, stderr = client.exec_command(command)
            for line in stdout:
                feedback(line.strip('\n'))

        def remote_copy(files, pipath):
            with client.open_sftp() as sftp:
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

        feedback('Copying files...')
        remote_command('rm -r -f ' + pipath)
        remote_command('mkdir -p ' + pipath)
        pkgdir = os.path.dirname(__file__)
        files = [
            # build files
            pkgdir + '/Release/deploy/CMakeLists.txt',
            pkgdir + '/Release/deploy/OpenBLASSetup.cmake',
            # python code
            pkgdir + '/vision/modelHelper.py',
            path + '.py',
            actuationpy,
            # native code
            path + 'PYTHON_wrap.cxx',
            path + 'PYTHON_wrap.h',
            pkgdir + '/Release/deploy/CallbackInterface.h',
            pkgdir + '/Release/deploy/ClockInterface.h',
            pkgdir + '/Release/deploy/CallbackInterface.tcc',
            path + '.i.h',
            path + '.o',
        ]
        remote_copy(files, pipath)
        feedback('Building...')
        remote_command('cd ' + pipath + '; mkdir build; cd build; cmake ..; make')
        feedback('Done')

        client.close()


def init_magics():
    try:
        ip = get_ipython()
        ip.register_magics(Arduino)
        ip.register_magics(RaspberryPi)
    except NameError:
        pass