"""
An ELL model from the Gallery
"""

import os.path


def _is_file_newer(file1, file2):
    return os.path.getmtime(file1) >= os.path.getmtime(file2)


class PretrainedModel:
    'A pretrained ELL model, which can be downloaded and compiled for a given device'

    def __init__(self, model_name):
        self.name = model_name
        self.local_path = None
        self.labels_path = None

    def download(self, local_path, cache=True):
        'Download the model from Github and unzip it'
        self.local_path = local_path
        if not os.path.exists(local_path):
            os.makedirs(local_path)
        local_file = os.path.join(local_path, self.name + '.ell.zip')
        self.labels_path = os.path.join(local_path, 'ILSVRC2012_labels.txt')
        if not cache or not os.path.exists(local_file):
            import urllib.request
            urllib.request.urlretrieve(
                'https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt',
                self.labels_path)
            zip_path, _ = urllib.request.urlretrieve(
                'https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/'
                + self.name + '/' + self.name + '.ell.zip', local_file)
            import zipfile
            with zipfile.ZipFile(zip_path, 'r') as zipref:
                zipref.extractall(local_path)
        return local_file

    def compile(self, platform):
        'Compile the model for a given platform'
        if self.local_path is None:
            raise Exception('must call download before compile')
        from . import ELL_Map
        path = os.path.join(self.local_path, self.name)
        ellmap = ELL_Map(path + '.ell')
        if not os.path.exists(path + '.ll') or _is_file_newer(
                path + '.ell', path + '.ll'):
            compiled = ellmap.Compile(platform, 'model', 'predict')
            compiled.WriteIR(path + '.ll')
            compiled.WriteSwigInterface(path + '.i')
            from .util.commands import run_llc, run_swig
            run_swig(path + '.i')
            run_llc(path + '.ll')
        else:
            print('compiled model up to date')

    def create_cmake_file(self):
        cmake_template = os.path.join(
            os.path.dirname(__file__), 'deploy', 'CMakeLists.python.txt.in')
        with open(cmake_template) as f:
            template = f.read()
        template = template.replace("@ELL_model@", self.name)
        template = template.replace("@Arch@", 'pi3')
        template = template.replace("@OBJECT_EXTENSION@", 'o')
        template = template.replace("@ELL_ROOT@", 'NotUsedOnLinux')
        cmake_file = os.path.join(self.local_path, "CMakeLists.txt")
        with open(cmake_file, 'w') as f:
            f.write(template)
        return cmake_file

    def files(self):
        myfiles = [
            'model.py', self.name + 'PYTHON_wrap.cxx',
            self.name + 'PYTHON_wrap.h', self.name + '.i.h', self.name + '.o'
        ]
        return [os.path.join(self.local_path, fn) for fn in myfiles] + [
            self.labels_path, self.create_cmake_file()
        ]
