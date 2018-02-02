from .. import package_dir
from .buildtools import EllBuildTools
import platform
import os
import sys
sys.path += [os.path.dirname(os.path.abspath(__file__)) ]

class CondaBuildTools(EllBuildTools):
    def __init__(self):
        self.pkgdir = package_dir()
        super(CondaBuildTools, self).__init__("", verbose=True)

    def swig_header_dirs(self):
        headerdir = self.pkgdir + '/headers'
        return [headerdir + p for p in ['/common', '/common/include', '/emitters']]

    def find_tools(self):
        try:
            # see if we are in development mode inside an ell folder.
            root = self.get_ell_build()
            super(CondaBuildTools, self).find_tools()
        except:
            # then we are in production mode, so use the conda installed tools
            if platform.node() == 'nbserver':  # Azure Notebooks
                condabin = self.GetAzureNotebookCondaDir()
            else:
                condabin = self.GetLocalAnacondaBinDir()

            self.compiler = None
            self.swigexe = os.path.join(condabin, 'swig')
            self.optexe = os.path.join(condabin, 'opt')
            self.llcexe = os.path.join(condabin, 'llc')
            self.blas = None

    def GetAzureNotebookCondaBinDir(self):
        paths = os.getenv("PATH").split(':')
        condapath = [x for x in paths if "anaconda" in x]
        if len(condapath):
            return condapath[0]            
        else:
            raise Exception("Cannot find anaconda in the PATH in your Azure Notebook environment")

    def GetLocalAnacondaBinDir(self):
        save_verbose = self.verbose
        self.verbose = False
        if platform.system() == 'Windows':
            condacmd = "conda.bat"

        output = self.run([condacmd, "env", "list"], print_output=False).split('\n')
        env = [x for x in output if "*" in x]
        if not env:
            raise Exception("Cannot find selected conda environment, found {}".format(output))

        condadir = env[0].split('*')[1].strip()

        self.verbose = save_verbose

        if platform.system() == 'Windows':
            condabin = os.path.join(condadir, 'Library', 'bin')
        else:
            condabin = os.path.join(condadir, 'bin')

        return condabin