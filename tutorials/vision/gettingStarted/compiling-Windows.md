
## Compiling on Windows

### Stage 1: Generating the model code

#### Generating the .map file

In order to generate code for a given model, we need ELL's representation of the model in a .map file. The demo Python scripts generate these files when they load the model for validation.

For instance, if you run `python darknetDemo.py` and it is able to load the Darknet reference model, the script will also save a `darknetReference.map` file in the same path.

*Note: The Darknet reference model is fairly small, we recommend you start with this first.  The build should take only a few minutes. For larger models (such as VGG16), the build can take a lot longer because of the size (we are working on ways to make this better).*

From Visual Studio's Solution Explorer, right click on the `compiled_darknetReference_host` project under tutorials\vision\compiled and build it.

Or run this command line:
```
cd ELL\build
cmake --build . --target compiled_darknetReference_host --config Release
```

Once the build is complete, the files will be generated under the `build\tutorials\vision\gettingStarted\compiled_darknetReference_host` location.

### Stage 2: Compiling the model code

In this stage, we will use the model object and wrapper code generated from Stage 1, build a Python module specific to that model, and run a demo Python script. We will use CMake to build the module.

#### Building the compiled Darknet module

Open a command prompt follow these steps:

Visual Studio 2015:
```
cd ELL\build\tutorials\vision\gettingStarted\compiled_darknetReference_host
mkdir build && cd build
cmake -G "Visual Studio 14 2015 Win64" ..
```

Visual Studio 2017:
```
cd ELL\build\tutorials\vision\gettingStarted\compiled_darknetReference_host
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" ..
```

To build the project, you can either run this from the command line:
```
cmake --build . --config Release
```

Or launch the CMake-generated `darknetReference.sln` and 
select the `Release x64` configuration and build.

#### Testing the compiled Darknet module

To test the newly-built model, you will need the same pre-requisites as the demo for importing the model (i.e. Python3, OpenCV, NumPy).  The `compiledDemoDarknet.py` script is basically a slightly stripped down version of the `darknetDemo.py` script.

1. Connect a webcam
2. From the `compiled_darknetReference_host` path, run this from a Python 3 environment (such as Anaconda or Miniconda):
```
    cd ELL\build\tutorials\vision\gettingStarted\compiled_darknetReference_host
    python compiledDarknetDemo.py
```

As with the non-compiled demo, you can optionally specify a camera number or image filename when you run the demo:
```
    python compiledDarknetDemo.py 1
    python compiledDarknetDemo.py impala.jpg
```

### Raspberry Pi 3

If the above test worked, then  you are ready to get things running on [Raspberry Pi 3](compiling-Pi3.md) by following very similar steps.

### More Information

For more detailed explanation on how all this works see [advanced compiling](compilingAdvanced.md).