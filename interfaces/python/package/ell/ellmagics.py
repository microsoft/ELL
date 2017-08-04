from IPython.core.magic import Magics, magics_class, cell_magic
import tempfile
import os
from .util.commands import run, print as feedback

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
        pipath = opts['pipath']
        dir = os.path.basename(path)
        actuationpy = self.save_cell(cell, 'actuation.py')

        def remote_command(command):
            run('plink -t -pw ' + self.password + ' -ssh ' + user + '@' + ip + ' ' + command)

        def remote_copy(files, pipath):
            run('pscp -q -pw ' + self.password + ' ' + ' '.join(files) + ' ' + user + '@' + ip + ':' + pipath)

        feedback('Copying files')
        remote_command('mkdir -p ' + pipath)
        files = [
            actuationpy,
            path + '.o',
            path + 'PYTHON_wrap.cxx',
            path + 'PYTHON_wrap.h',
        ]
        remote_copy(files, pipath)
        feedback('Done')


def init_magics():
    try:
        ip = get_ipython()
        ip.register_magics(Arduino)
        ip.register_magics(RaspberryPi)
    except NameError:
        pass