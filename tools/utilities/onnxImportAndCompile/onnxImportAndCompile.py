import os
import sys
import argparse
import shutil
import subprocess
import validators
import urllib.request

ONNX_DEFAULT_PATH = "model.onnx"
ELL_MODEL_PATH = "model.ell"

try:
    ELL_ROOT = os.environ['ELL_ROOT']
    print("ELL_ROOT:", ELL_ROOT)
except (ValueError, RuntimeError, TypeError, NameError, KeyError):
    print("ERROR: Environment variable ELL_ROOT must be specified")
    exit(-1)

importOnnxScript = os.path.join(ELL_ROOT, 'tools', 'importers', 'onnx', 'onnx_import.py')
wrapScript = os.path.join(ELL_ROOT, 'tools', 'wrap', 'wrap.py')

def get_onnx_file(onnxarg):
    # Firstly, if no onnx arg was provided, check for the default "model.onnx"
    if (not onnxarg):
        onnxpath = os.path.abspath(ONNX_DEFAULT_PATH)
        if (os.path.isfile(onnxpath)):
            return onnxpath
        return None

    # Look for the onnx path on the local file system
    onnxpath = os.path.abspath(onnxarg)
    if (os.path.isfile(onnxpath)):
        # Found it on local file system
        return onnxpath

    # Check if the onnx arg is a URL
    if (validators.url(onnxarg)):
        # The onnx arg is a url! Let's try to download it
        try:
            urllib.request.urlretrieve(onnxarg, ONNX_DEFAULT_PATH)
            return ONNX_DEFAULT_PATH
        except:
            return None

    return None

def run_onnx_ell_import(onnx_local_path):
    print("Running ONNX import on file:", onnx_local_path)
    cmd = F"python {importOnnxScript} {onnx_local_path}"
    print(cmd)
    subprocess.call(cmd, shell=True)
    return ELL_MODEL_PATH

def run_ell_wrap(ell_model_path, ell_language, target):
    print("Running ELL wrap for target:", target)
    cmd = F"python {wrapScript} --model_file {ell_model_path} --language {ell_language} --target {target}"
    print(cmd)
    subprocess.call(cmd, shell=True)

def zip_directory(zip_path, dir_path):
    shutil.make_archive(zip_path, format="zip", root_dir=dir_path)

if (not os.path.isfile(importOnnxScript) or not os.path.isfile(wrapScript)):
    print("ERROR: ELL tools not found")

def main(argv):
    parser = argparse.ArgumentParser(
        description="Converts ONNX model to device-ready ELL model")
    parser.add_argument(
        "--onnx",
        help="Input model file (onnx)")
    parser.add_argument(
        "--lang",
        help="Output language for wrap script ('cpp', 'python')")
    parser.add_argument(
        "--target",
        required=True,
        help="The target platform(s) for the wrap script, as a comma-separted list (i.e. 'pi0,pi3')")

    args = parser.parse_args()

    onnx_local_path = get_onnx_file(args.onnx)
    if (not onnx_local_path):
        print("ONNX file not found. Please save your ONNX file as {ONNX_DEFAULT_PATH}, or path a valid local or URL path in as --onnx argument.")

    ell_model_path = run_onnx_ell_import(onnx_local_path)

    target_list = args.target.split(",")
    ell_language = "python"
    for target in target_list:
        run_ell_wrap(ell_model_path, ell_language, target)
        zip_directory(target, target)

if __name__ == '__main__':
    main(sys.argv[1:])