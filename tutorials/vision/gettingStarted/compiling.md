## Compiling ELL Models 

ELL provides a very cool neural net model compiler that can target a specific platform and produce optimized
code for that platform.  This helps to make your models run faster on low end hardware.

So we will take the models we loaded, either from [Darknet](darknet.md) or [CNTK](cntk.md) and 
show you how to compile them on your desktop machine (either Windows, Linux, or Mac), and also for Raspberry Pi.

*Important:* Before proceeding, first run the demo Python script for the model of your choice (cntkDemo.py or darknetDemo.py). Besides loading the models, each script will convert the neural net to a .map file that represents the model in ELL's format. Depending on the neural network, the file can get quite large.

*Important:* Make sure you have already built ELL in the `Release` configuration, so that you have the ELL compilation tools ready. 

When you build ELL you will also get a copy of the tutorials/vision/gettingStarted folder copied to your `build` folder with some additional build generated scripts.

### Overview of the building process

Once a model has been imported into ELL, compiling an ELL model is a two-stage process:

1. Generating the model code from the imported model (e.g. darknetReference.map) to generate object and wrapper code for the target device. This stage is run on a desktop machine and performs the following steps:
    * Compile the model from ELL, emitting LLVM IR
    * Compiling the LLVM IR, generating model object code
    * Generate the SWIG wrapper code (so you can invoke your model using Python)

2. Compiling the model code on target device into a Python module. 

    *Note: Stage 2 can also be run on the desktop machine, if you have the cross-compilation toolchain available. Cross-compilation is a more advanced scenario that we plan to cover in a future section / tutorial.*

With the above background, let's get started. The compilation workflow is similar for each model, so we'll use the Darknet reference model in our walkthrough.

### Compiling

Please choose your build platform:

- [Compiling on Windows](compiling-Windows.md)
- [Compiling on Linux](compiling-Linux.md)
- [Compiling for Raspberry Pi](compiling-Pi3.md)

    