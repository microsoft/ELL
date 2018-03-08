"""
An ELL model from the Gallery
"""

import os
import os.path
import sys
import numpy as np
from .condabuildtools import CondaBuildTools

_buildtools = CondaBuildTools()

def _is_file_newer(file1, file2):
    return os.path.getmtime(file1) >= os.path.getmtime(file2)

def _is_windows(target):
    import platform
    return target.lower() == 'host' and platform.system() == 'Windows' or target == 'windows'


class PretrainedModel:
    """A pretrained ELL model, which can be downloaded and compiled for a given device"""

    def __init__(self, model_name):
        self.model_name = model_name
        self.name = model_name
        self.local_path = None
        self.labels_path = None
        self.model = None
        self.deploy_dir = os.path.join(os.path.dirname(__file__), '..', 'deploy')

    def rename(self, new_name):
        self.name = new_name

    def download(self, local_path, rename=None, cache=True):
        """Download the model from Github and unzip it"""
        import urllib.request
        
        self.local_path = local_path
        os.makedirs(local_path, exist_ok=True)
        local_file = os.path.join(local_path, self.name + '.ell')
        local_zip_file = os.path.join(local_path, self.name + '.ell.zip')
        self.labels_path = os.path.join(local_path, 'categories.txt')
        if not cache or not os.path.exists(self.labels_path):
            urllib.request.urlretrieve(
                'https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt',
                self.labels_path)
            
        if not cache or not os.path.exists(local_file):
            print('downloading model ' + self.model_name + ' ...', flush=True)
            zip_path, _ = urllib.request.urlretrieve(
                'https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/'
                + self.model_name + '/' + self.model_name + '.ell.zip', local_zip_file)
            import zipfile
            with zipfile.ZipFile(zip_path, 'r') as zipref:
                ellfiles = [n for n in zipref.namelist() if n[-4:] == '.ell']
                if not ellfiles:
                    raise Exception("file contains no ell model")
                zipref.extractall(local_path)
                unzipped_file = os.path.join(local_path, ellfiles[0])
                if local_file != unzipped_file:
                    os.rename(unzipped_file, local_file)

        if rename:
            from shutil import copyfile
            self.rename(rename)
            renamed_file = os.path.join(local_path, self.name + '.ell')
            if local_file != renamed_file:
                copyfile(local_file, renamed_file)
            local_file = renamed_file

        return os.path.abspath(local_file)

    def compile(self, target):
        """Compile the model for a given target platform"""
        if self.local_path is None:
            raise Exception('must call download before compile')
        print('compiling...', flush=True)
        from .. import model
        inpath = os.path.join(self.local_path, self.name)
        outdir = os.path.join(self.local_path, target)
        if not os.path.exists(outdir):
            os.makedirs(outdir)
        outpath = os.path.join(outdir, self.name)
        ellmap = model.Map(inpath + '.ell')
        cmakefile = os.path.join(outdir, 'CMakeLists.txt')
        if not os.path.exists(cmakefile) or _is_file_newer(inpath + '.ell', cmakefile) \
           or not os.path.exists(outpath + '.bc'):
            compiled = ellmap.Compile(target, self.name, 'model_predict',
                dtype=np.float32)
            compiled.WriteBitcode(outpath + '.bc')
            compiled.WriteSwigInterface(outpath + '.i')

            if not os.path.exists(outpath + '.bc'):
                raise Exception("compile failed to produce output file: " +
                    os.path.exists(outpath + '.bc')) 

            if _buildtools.swig(outdir, self.name, 'python') is None:
                return None
            out_file = _buildtools.opt(outdir, outpath + '.bc') 
            if out_file is None:
                return None
            out_file = _buildtools.llc(outdir, out_file, target) 
            if out_file is None:
                return None
            return self.create_cmake_file(target)
        else:
            print('compiled model up to date')
            return cmakefile

    def build(self, target='host'):
        orig_dir = os.getcwd()
        try:
            cmake_file = self.compile(target)
            if cmake_file is None:
                return None
            print('building...', flush=True);
            os.chdir(os.path.dirname(cmake_file))
            import shutil
            pkg_dir = self.deploy_dir
            shutil.copyfile(os.path.join(pkg_dir, 'OpenBLASSetup.cmake'), 'OpenBLASSetup.cmake')
            if not os.path.exists('include'):
                shutil.copytree(os.path.join(pkg_dir, 'include'), 'include')
            if not os.path.exists('tcc'):
                shutil.copytree(os.path.join(pkg_dir, 'tcc'), 'tcc')
            if _is_windows(target):
                _buildtools.run(['cmake', '-G', 'Visual Studio 14 2015 Win64', '-DPROCESSOR_HINT=haswell', '.'], shell=True)
                _buildtools.run(['cmake', '--build', '.', '--config', 'Release'], shell=True)
            else:
                _buildtools.run('cmake .', shell=True)
                _buildtools.run('make', shell=True)
        finally:
            os.chdir(orig_dir)

    def create_cmake_file(self, target):
        cmake_template = os.path.join(self.deploy_dir, 'CMakeLists.python.txt.in')
        with open(cmake_template) as f:
            template = f.read()
        
        template = template.replace("@ELL_outdir@", self.name)
        template = template.replace("@ELL_model@", self.name)
        template = template.replace("@ELL_model_name@", self.name)

        template = template.replace("@Arch@", target)
        template = template.replace("@OBJECT_EXTENSION@", 'obj')
        template = template.replace("@ELL_ROOT@", 'NotUsedOnLinux')
        cmake_file = os.path.join(self.local_path, target, "CMakeLists.txt")
        with open(cmake_file, 'w') as f:
            f.write(template)
        return cmake_file

    def files(self, target):
        myfiles = [
            'model.py',
            self.name + 'PYTHON_wrap.cxx',
            self.name + 'PYTHON_wrap.h',
            self.name + '.i.h',
            self.name + '.obj'
        ]
        return [os.path.join(self.local_path, target, f) for f in myfiles] + [
            self.labels_path, self.create_cmake_file(target)
        ]

    def load(self):
        model_dir = os.path.abspath(os.path.join(self.local_path, 'host')) # can only load 'host'
        if not os.path.exists(model_dir):
            raise Exception("must build for target platform 'host' before loading")
        sys.path.append(model_dir)
        if _is_windows('host'):
            sys.path.append(os.path.join(model_dir, 'Release'))
        self.model = __import__(self.name)
        return self.model

    def prepare_image(self, image, reorderToRGB = False):
        """ Prepare an image for use with a model. Typically, this involves:
        - Resize and center crop to the required width and height while preserving the image's aspect ratio.
        Simple resize may result in a stretched or squashed image which will affect the model's ability
        to classify images.
        - OpenCV gives the image in BGR order, so we may need to re-order the channels to RGB.
        - Convert the OpenCV result to a std::vector<float> for use with ELL model
        """
        import cv2
        input_shape = self.model.get_default_input_shape()
        required_width = input_shape.columns
        required_height = input_shape.rows
        if image.shape[0] > image.shape[1]:  # Tall (more rows than cols)
            row_start = int((image.shape[0] - image.shape[1]) / 2)
            row_end = row_start + image.shape[1]
            col_start = 0
            col_end = image.shape[1]
        else:  # Wide (more cols than rows)
            row_start = 0
            row_end = image.shape[0]
            col_start = int((image.shape[1] - image.shape[0]) / 2)
            col_end = col_start + image.shape[0]
        # Center crop the image maintaining aspect ratio
        cropped = image[row_start:row_end, col_start:col_end]
        # Resize to model's requirements
        resized = cv2.resize(cropped, (required_height, required_width))
        # Re-order if needed
        if not reorderToRGB:
            resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
        # Return as a vector of floats
        result = resized.astype(np.float).ravel()
        return result
