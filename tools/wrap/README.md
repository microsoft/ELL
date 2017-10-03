## Wrap.py

**Usage:** wrap.py label_file model_file [-lang name|-target name|-outdir name|-v]

This tool wraps a given ELL model in a CMake C++ buildable project that builds a language specific module that makes it possible for you to invoke the ELL model on a given target platform from your specified programming language.

The supported languages are:
- `python`   (default)
- `cpp`

The supported target platforms are:
- `pi3`      raspberry pi 3
- `aarch64`  arm64 Linux, works on Qualcomm DragonBoards
- `host`     (default) your host computer architecture

See [Getting Started with Image Classification on the RaspberryPi](../../../docs/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/index.md) for an example of how to use this tool

### What does this tool do exactly?

If you run the tool with the additional `-v` command line option you will see the steps it is taking.  

The output from this for building a Python callable interface on an ImageNet model for Raspberry pi 3 is shown below.
You will see the same commands on Linux or Mac only with '/' instead of '\\' in the path names.

#### Copy Files

First it copies some common files that are needed to build the C++ project:
````
copy "d:\git\ELL\CMake/OpenBLASSetup.cmake" "pi3\OpenBLASSetup.cmake"
copy "d:\git\ELL\interfaces/common/include/CallbackInterface.h" "pi3\include\CallbackInterface.h"
copy "d:\git\ELL\libraries/emitters/include/ClockInterface.h" "pi3\include\ClockInterface.h"
copy "d:\git\ELL\interfaces/common/tcc/CallbackInterface.tcc" "pi3\tcc\CallbackInterface.tcc"
````

### Templated CMakeLists.txt for Python

Included in the templates folder is a CMakeLists.python.txt.in template.
The final CMakeLists.txt file is generated from this template by filling
in some text parameters:


<table style="border:1px solid #A0A0A0">
<tr><td>ELL_model</td><td>of the model</td></tr>
<tr><td>Arch</td><td>target architecture, e.g. host, pi3</td></tr>
<tr><td>OBJECT_EXTENSION</td><td>obj or o</td></tr>
<tr><td>ELL_ROOT</td><td>location of ELL git repo</td></tr>
</table>

These parameters are filled and the resulting CMakeLists.txt file is written to the output folder.

### Templated CMakeLists.txt for C++

Included in the templates folder is a CMakeLists.cpp.txt.in template.
The final CMakeLists.txt file is generated from this template by filling
in some text parameters:

<table style="border:1px solid #A0A0A0"">
<tr><td>ELL_model</td><td>of the model</td></tr>
<tr><td>Arch</td><td>target architecture, e.g. host, pi3</td></tr>
<tr><td>OBJECT_EXTENSION</td><td>obj or o</td></tr>
<tr><td>ELL_ROOT</td><td>location of ELL git repo</td></tr>
</table>

These parameters are filled and the resulting CMakeLists.txt file is written to the output folder.
This describes the compiled model to CMake so that it can be referenced in C++ projects, such as a calling application.

#### Compile Model

Next it invokes the ell compiler to compile the given model.  The command line looks like this:
````
compile -imap ImageNet.ell -cfn predict -cmn ImageNet --bitcode --swig --blas true --target pi3 -od pi3
````
This compiler is created by the ELL build and you will find it in `~/git/ELL/build/bin` and on Windows it will be in `d:\git\ELL\ELL\build\bin\release\`.

#### Optimizing the code

Next, wrap invokes the optimizing LLVM tool `opt` on the output from compile. The command looks something like:
````
opt.exe pi3\ImageNet.bc -o pi3\ImageNet.opt.bc -O3
````

#### Generating SWIG interfaces

For language targets other than C++, ELL uses SWIG to generate the stubs necessary to convert between programming languages:
````
swig -python -c++ -Fmicrosoft -py3 -outdir pi3 -ID:\git\ELL\interfaces/common -ID:\git\ELL\interfaces/common/include -ID:\git\ELL\libraries/emitters/include -o pi3\ImageNetPYTHON_wrap.cxx pi3\ImageNet.i
````
On Windows we include SWIG in a nuget package, so you will find it in `d:\git\ELL\external\swigwintools.3.0.12\tools\swigwin-3.0.12\`

#### Targeting the Model

Finally it uses LLC to cross-compile the model so that code runs on your specified target platform.  

````
llc pi3\ImageNet.bc -o pi3\ImageNet.obj -filetype=obj -O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic
````
On Windows LLC is also included in a NuGet package so you will find it in `d:\git\ELL\external\LLVMNativeWindowsLibs.x64.<llvm_version>\build\native\tools\`,
where <llvm_version> is version of LLVM bundled in the NuGet package.