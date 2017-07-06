## Compiling ELL Models

ELL provides a very cool neural net model compiler that can target a specific platform and produce optimized
code for that platform.  This helps to make your models run faster on low end hardware.

So we will take the models we loaded, either from [Darknet](darknet.md) or [CNTK](cntk.md) and compile them
for Raspberry Pi.

*Important:* Before proceeding, first run the demo Python script for the model of your choice (cntkDemo.py or darknetDemo.py). Besides loading the models, each script will convert the neural net to a .map file that represents the model in ELL's format. Depending on the neural network, the file can get quite large.

*Important:* Make sure you have already built ELL in the `Release` configuration, so that you have the ELL compilation tools ready. 

When you build ELL you will also get a copy of the tutorials/vision/gettingStarted folder copied to your `build` folder with some additional build generated scripts.

### Overview of the building process

Once a model has been imported into ELL, compiling an ELL model is a two-stage process:

1. Generating the model code from the imported model (e.g. darknetReference.map) to generate object and wrapper code for the target device. This stage is run on a desktop machine and performs the following steps:
    * Compile the model from ELL, emitting LLVM IR
    * Compiling the LLVM IR, generating model object code
    * Generate the SWIG wrapper code (for Python bindings)

2. Compiling the model code on target device into a Python module. 

    *Note: Stage 2 can also be run on the desktop machine, if you have the cross-compilation toolchain available. Cross-compilation is a more advanced scenario that we plan to cover in a future section / tutorial.*

With the above background, let's get started. The compilation workflow is similar for each model, so we'll use the Darknet reference model in our walkthrough.

### Stage 1: Generating the model code

First, re-run CMake. This will detect that the model files e.g. `darknetReference.map` are now present, and will create targets for creating the necessary files for compiling on various device targets. 

From the command window, in the `ELL\build` folder:
```
cmake ..
```
If the targets are generated, you should see these messages in the command window output:
```
-- Generated target compiled_darknetReference_host
-- Generated target compiled_darknetReference_pi3
```
If you don't see messages similar to the above, CMake could not find the .map file under `build\tutorials\vision\gettingStarted`. Maybe try re-running darknetDemo.py successfully again.

If you had ELL.sln open, Visual Studio will prompted you to reload the solution. Otherwise, open ELL.sln from the `build` folder.  In Solution Explorer, look under `tutorials`, `vision`, and then `compiled`, and you should see new projects for the compiled models on the host (Windows/Linux/Mac) and Raspberry Pi 3.

*Note: The Darknet reference model is fairly small, we recommend you start with this first.  The build should take only a few minutes. For larger models (such as VGG16), the build can take a lot longer because of the size (we are working on ways to make this better).*

#### Windows 64 bit

From Visual Studio's Solution Explorer, right click on the 'compiled_darknetReference_host' project under tutorials\vision\compiled and build it.

Once the build is complete, the files will be generated under the `build\tutorials\vision\gettingStarted\compiled_darknetReference_host` location.

#### Linux / Mac

Generate the compiled_darknetReference_host files by building its target from ELL's build directory:

```
cd ELL/build
cmake ..
make compiled_darknetReference_host
```

#### Raspberry Pi 3

##### Generating Raspberry Pi 3 model files from a Windows host

From Visual Studio's Solution Explorer, right click on the 'compiled_darknetReference_pi3' project under tutorials\vision\compiled and build it.

Once the build is complete, copy the  `build\tutorials\vision\gettingStarted\compiled_darknetReference_pi3` folder to the Raspberry Pi 3. If you have [SSH enabled](https://www.raspberrypi.org/documentation/remote-access/ssh/) on the Raspberry Pi 3, you can use [Secure Copy](https://www.raspberrypi.org/documentation/remote-access/ssh/scp.md). If copying from a Windows system, the [WinSCP](https://winscp.net) application works well. 

##### Generating Raspberry Pi 3 model files from a Linux / Mac host

```
cd ELL/build
cmake ..
make compiled_darknetReference_pi3
```

Once the build is complete, copy the  `build/tutorials/vision/gettingStarted/compiled_darknetReference_pi3` folder to the Raspberry Pi 3. If you have [SSH enabled](https://www.raspberrypi.org/documentation/remote-access/ssh/) on the Raspberry Pi 3, you can use [Secure Copy](https://www.raspberrypi.org/documentation/remote-access/ssh/scp.md). 

For example (from Linux / Mac host, replacing IP_ADDRESS with your Raspberry Pi's IP address):
```
cd ELL/build/tutorials/vision/gettingStarted/compiled_darknetReference_pi3
tar zcvf compiled_darknetReference_pi3.tgz compiled_darknetReference_pi3
scp compiled_darknetReference_pi3.tgz pi@IP_ADDRESS:/home/pi/compiled_darknetReference_pi3.tgz
```

### Stage 2: Compiling the model code

In this stage, we will use the model object and wrapper code generated from Stage 1, build a Python module specific to that model, and run a demo Python script. We will use CMake to build the module.

#### Windows 64 bit

##### Building the compiled Darknet module

Open a command prompt and navigate to the `build\tutorials\vision\gettingStarted\compiled_darknetReference_host` folder. Create a `build` subdirectory and change to that directory to run CMake:

Visual Studio 2015:
```
mkdir build && cd build
cmake -G "Visual Studio 14 2015 Win64" ..
```

Visual Studio 2017:
```
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
```

To build the project, you can either run this from the command line:
```
cmake --build . --config Release
```

Or launch the CMake-generated `darknetReference.sln`:
```
darknetReference.sln
```
Select the `Release x64` configuration and build.

##### Testing the compiled Darknet module

To test the newly-built model, you will need the same pre-requisites as the demo for importing the model (i.e. Python3, OpenCV, NumPy).  The `compiledDemoDarknet.py` script is basically a slightly stripped down version of the `darknetDemo.py` script.

1. Connect a webcam
2. From the `compiled_darknetReference_host` path, run this from a Python 3 environment (such as Anaconda or Miniconda):
```
    python compiledDarknetDemo.py
```

#### Linux / Mac

##### Building the compiled Darknet module

Navigate to the `build/tutorials/vision/gettingStarted/compiled_darknetReference_host` folder. Create a `build` subdirectory and change to that directory:

```
cd ELL/build/tutorials/vision/gettingStarted/compiled_darknetReference_host
mkdir build
cd build
cmake ..
make
```

Note: The difference between Stage 1 and Stage 2 is in *where* you run `cmake` and `make` from. In Stage 1, you were generating the target and files needed for Stage 2. In Stage 2, you proceed to build the generated files into the Python module.

##### Testing the compiled Darknet module

To test the newly-built model, you will need the same pre-requisites as the demo for importing the model (i.e. Python3, OpenCV, NumPy).  The `compiledDemoDarknet.py` script is basically a slightly stripped down version of the `darknetDemo.py` script.

1. Connect a webcam
2. From the `compiled_darknetReference_host` path, run this from a Python 3 environment (such as Anaconda or Miniconda):
```
    python compiledDarknetDemo.py
```

#### Raspberry Pi 3

This portion of the tutorial presumes you are familiar with setting up a Raspberry Pi. If not, the [official documentation](https://www.raspberrypi.org/documentation/) is a great place to start.

The steps below are tested on a Raspberry Pi 3, running a current version of 32-bit Raspbian.  We'll be testing with other versions of Raspberry Pis and will update the tutorial as and when they are ready.

##### Hardware / Component Setup

To run the demo, you will need:
* A camera to capture video, such as a USB webcam or a [Raspberry Pi Camera Module v2](https://www.raspberrypi.org/products/camera-module-v2/). We recommend standard USB webcams because they usually don't require additional driver configuration.
   * If using the Raspberry Pi Camera Module, you will need to enable the V4L driver so that OpenCV can use it. More details are [here](https://www.raspberrypi.org/forums/viewtopic.php?t=62364).

* A display to see video, such as an HDMI monitor or a touchscreen display.

##### Software Setup

1. Build pre-requisites:

   * CMake and OpenBLAS:

    ```
    pi@raspberrypi:~ $ sudo apt-get update
    pi@raspberrypi:~ $ sudo apt-get install -y cmake libopenblas-dev
    ```

   * Python 3.4 from Miniconda3. The default options should work, although we recommend picking "yes" when you get the prompt to add Miniconda to the PATH in /home/pi/.bashrc.

    ```
    pi@raspberrypi:~ $ wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
    pi@raspberrypi:~ $ chmod +x Miniconda3-latest-Linux-armv7l.sh
    pi@raspberrypi:~ $ ./Miniconda3-latest-Linux-armv71.sh
    <Follow prompts to install, we recommend [yes] for adding Miniconda3 to the PATH>
    pi@raspberrypi:~ $ source ~/.bashsrc # if you selected [yes] to add to the PATH
    pi@raspberrypi:~ $ conda create --name py34 python=3
    pi@raspberrypi:~ $ source activate py34
    ```

2. Demo pre-requisites:

   * Install NumPy and OpenBLAS:
    ```
    (py34) pi@raspberrypi:~ $ conda install numpy
    (py34) pi@raspberrypi:~ $ conda install openblas
    ```
   * Install the OpenCV arm7l package from the `microsoft-ell` channel:
    ```
    (py34) pi@raspberrypi:~ $ conda install -c microsoft-ell opencv
    ```

##### Building the compiled Darknet module

If you've copied the compiled model's folder (containing the generated object and wrapper code) to the Raspberry Pi 3, proceed to building the module:

```
pi@raspberrypi:~ $ tar zxvf compiled_darknetReference_pi3.tgz # if you had scp'ed the tarball earlier
pi@raspberrypi:~ $ cd compiled_darknetReference_pi3
pi@raspberrypi:~ $ mkdir build && cd build
pi@raspberrypi:~ $ cmake ..
pi@raspberrypi:~ $ make
```

This will build a `_darknetReference.so` Python module that our compiled demo script will use.

##### Testing the compiled Darknet module

Run the compiled demo script from the `compiled_darknetReference_pi3` folder and a Python 3.4 Miniconda environment:
```
(py34) pi@raspberrypi:~ $ cd compiled_darknetReference_pi3
(py34) pi@raspberrypi:~ $ python compiledDarknetDemo.py
```

### Compiling ELL models: how this works

This section describes what happens in command line terms when you run the CMake commands on a Windows PC to compile the `darknetReference.map` file into the `darknetReference` Python module.

*Linux instructions are similar except with forward slashes for path separators,
and invoking gcc or clang instead of the MSVC `cl` and `link` commands respectively.*

#### Windows 64 bit

First, we need to establish where your ELL root directory is, if you are in the build/tutorials/vision/gettingStarted folder then this should be the root:
```
set ELL_ROOT=..\..\..\..
```    
Next we need to be able to find the ELL compiler which lives here:
```
set PATH=%PATH%;%ELL_ROOT%\build\bin\Release
```
Now run this to compile the darknet model to LLVM intermediate representation (IR).
```
compile -imap darknetReference.map -cfn darknetReference -of darknetReference.ll
```
*Note:* this may take a minute or more.

We need to also generate the Python wrapper code for the model:
```
compile -imap darknetReference.map -cfn darknetReference -cmn darknetReference -o swig -of darknetReference.i
```
*Note:* this may also take a minute or more.

Next we need to run SWIG to generate the C++ Python wrappers.  If you installed SWIG manually then it is probably
already in your PATH, if not then you will find it here (on Windows):
```
set PATH=%PATH%;%ELL_ROOT%\external\swigwintools.3.0.12\tools\swigwin-3.0.12
```
Then run:
```
swig -python -modern -c++ -Fmicrosoft -py3 -outdir . -c++ -I%ELL_ROOT%/interfaces/common/include -I%ELL_ROOT%/interfaces/common -I%ELL_ROOT%/libraries/emitters/include -o darknetPYTHON_wrap.cxx darknetReference.i
```
This should be quick.  Next we run `llc` to compile the IR language generated by compile step above
into a .obj linkable module.  This means we need to be able to find llc:
```
set PATH=%PATH%;%ELL_ROOT%\external\LLVMLibs.3.9.0.1\build\native\tools\
```
Then run this:
```
llc -filetype=obj darknet.ll -march x86-64
```
*Note:* the machine architecture we chose there means you need to be running it on 64 bit Windows.
`llc` supports many other targets, including what we need to run on Raspberry Pi.

Now we can build the Python module using the MSVC compiler.  This means you need to open the
Visual Studio 2015 X64 Native Tools Command Prompt, to ensure you are building 64 bit executable.

The compiler and linker will need to be able to find your Python 3.6 SDK.  If you type `where python`
you will see where the Python executable lives, then set the following variable to point to that location:
```
set PY_ROOT=d:\Continuum\Anaconda2\envs\py36
```
In my case I was using Anaconda.  Now you can run the Visual Studio C++ compiler to compile the Python wrapper:
```
cl /I%PY_ROOT%\include /I%ELL_ROOT%\interfaces\common\include\ /I%ELL_ROOT%\libraries\emitters\include\ /c /EHsc /MD darknetPYTHON_wrap.cxx
```
And we finish up with the Visual Studio C++ linker to produce the Python-loadable module.
The linker may also need to find the libOpenBLAS library which lives.   OpenBLAS is a library that is optimized for specific types of CPUs, right down to the CPU model number.  So you need to pick the right processor type.
For Intel, we support either haswell or sandybridge.  Note that Ivy Bridge is compatible with Sandy Bridge, and Broadwell is compatible with Haswell.  When you ran `cmake` for ELL part of the cmake output will tell you what processor family you have, you should see some output like this:
```
-- Processor family: 6, model: 79
-- Using OpenBLAS compiled for haswell
```
So this means I need to use this version of OpenBLAS:
``` 
set OPENBLAS=%ELL_ROOT%\external\OpenBLASLibs.0.2.19.2\build\native\x64\haswell\lib
```
Then run the linker:
```
link darknetPYTHON_wrap.obj darknet.obj %PY_ROOT%\libs\python35.lib %OPENBLAS%\libopenblas.dll.a /MACHINE:x64 /SUBSYSTEM:WINDOWS /DLL /DEBUG:FULL /PDB:_darknet.pdb /OUT:_darknet.pyd
```
#### Running the Compiled Model

Ok, now that _darknet.pyd exists, we can load it up into Python and see if it works. From your
Anaconda Python 3.6 environment run this:
```
python compiledDarknetDemo.py
```
If this fails to load, it could be because it is failing to find the OpenBLAS library, so do this:
```
set PATH=%PATH%;%OPENBLAS%
```
(using the OPENBLAS variable we defined earlier on this page) then try again.

One thing you should notice is that this compiled version of darknet loads instantly and runs quickly.
