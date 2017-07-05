import sys
import os

# see if we are in the build directory already
currentPath = os.path.dirname(os.path.abspath(__file__))
os.chdir(currentPath)  # make sure we are in this directory.

buildDir = "./../../../../build"
ellPyPath = buildDir + "/interfaces/python/ELL.py"
if (not os.path.exists(ellPyPath)):
    # see if user is in the source directory
    buildDir="./../../../build"
    ellPyPath=buildDir + "/interfaces/python/ELL.py"

if (not os.path.exists(ellPyPath)):
    print("Could not find '" + ellPyPath + "', did you follow the ELL Python Binding build instructions?")
    sys.exit(1)    

if sys.platform == 'win32':
    releasePath = buildDir + "/interfaces/python/Release"
    if (not os.path.exists(releasePath)):
        print("Could not find '" + releasePath + "', did you build the Release configuration as per the README instructions?")


sys.path.append(buildDir + '/interfaces/python')
sys.path.append(buildDir + '/interfaces/python/utilities')
sys.path.append(buildDir + '/interfaces/python/Release')
sys.path.append(buildDir + '/tools/importers/CNTK')
sys.path.append(buildDir + '/tools/importers/darknet')

try:
    __import__("ELL")
    __import__("ell_utilities")
except:    
    print("====================================================================")
    print("ELL module is not loading")
    print("It is possible that you need to add LibOpenBLAS to your system path (See Install-*.md) from root of this repo")