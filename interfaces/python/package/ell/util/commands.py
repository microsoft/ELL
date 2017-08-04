from IPython.core.display import display, HTML
import subprocess
import os.path


def print(text):
    display(HTML('<pre style="font-size:80%; line-height:110%">' + text + '</pre>'))

def run(command, print_output=True, echo=False):
    try:
        if echo:
            print('<b>' + command + '</b>')

        proc = subprocess.Popen(command, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=0)
        output = '';
        for line in proc.stdout:
            text = line.decode('utf-8')
            output += text
            if print_output:
                print(text)
        return output 
    except FileNotFoundError:
        if print_output:
            print("cannot run command: " + command)
        return None

condadir = None

def get_conda_dir():
    global condadir
    if not condadir:
        condadir = run("conda info --root", False).rstrip('\n\r')
    return condadir

def run_swig(filepath):
    # swig -python -modern -c++ -Fmicrosoft -py3 -outdir . -c++ -I%ELL_ROOT%/interfaces/common/include -I%ELL_ROOT%/interfaces/common -I%ELL_ROOT%/libraries/emitters/include -o _darknetReferencePYTHON_wrap.cxx darknetReference.i
    from .. import package_dir
    headerdir = package_dir() + '/Release/headers'
    command = ' '.join([
        get_conda_dir() + '/pkgs/swig-3.0.12-h0d4ecd4_0/Library/bin/swig',
        '-python',
        '-modern',
        '-c++',
        '-Fmicrosoft',
        '-py3',
        '-outdir ' + os.path.dirname(filepath),
        '-I' + headerdir + '/common',
        '-I' + headerdir + '/common/include',
        '-I' + headerdir + '/emitters',
        '-o ' + os.path.splitext(filepath)[0] + 'PYTHON_wrap.cxx',
        filepath
    ])
    run(command, True, True)

def run_llc(filepath):
    # llc -filetype=obj _darknetReference.ll -O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic
    command = ' '.join([
        get_conda_dir() + '/pkgs/llvm-3.9.1-hb79d290_0/Library/bin/build/native/tools/llc',
        '-filetype=obj',
        '-O3',
        '-mtriple=armv7-linux-gnueabihf',
        '-mcpu=cortex-a53',
        '-relocation-model=pic',
        filepath
    ])
    run(command, True, True)
