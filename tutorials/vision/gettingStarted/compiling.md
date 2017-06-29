## Compiling ELL Models

ELL provides a very cool neural net model compiler that can target a specific platform and produce optimized
code for that platform.  This helps to make your models run faster on low end hardware.

So we will take the models we loaded, either from [darknet](darknet.md) or [cntk](cntk.md) and compile them
for Raspberry Pi.

*Important* Before proceeding, first run the demo python script for the model of your choice (cntkDemo.py or darknetDemo.py). Besides loading the models, each script will convert the neural net to a .map file that represents the model in ELL's format. Depending on the neural network, the file can get quite large.

*Important* Make sure you have already built ELL in the Release configuration, so that you have the ELL compilation tools ready.

The steps below use a darknet model as an example.

First, we compile the model to LLVM intermediate representation (IR). 

For example:

    ..\..\..\bin\Release\compile -imap darknet.map -cfn darknet -m clockStep -of darknet.ll

We will be running python on the Raspberry Pi to use the model. So, we need to generate the python wrapper code for the model. 

For example:

    ..\..\..\bin\Release\compile -imap darknet.map -cfn darknet -m clockStep -o swig -of darknet.i

Both commands above will take a while to run because of the sizes of the neural networks. We are actively working on ways to reduce the time... stay tuned.

Still to add:
- Run SWIG 
swig.exe -python -c++ -Fmicrosoft -py3 -outdir output -c++ ^
-I<ELL_root>/interfaces/common/include ^
-I<ELL_root>/interfaces/common ^
-I<ELL_root>/libraries/emitters/include ^
-o output/darknetPYTHON_wrap.cxx ^
darknet.i

- Compile IR (need to specify target triple)
llc darknet.ll -o darknet.o -filetype=obj -relocation-model=pic

- Generate CMakeLists.txt for module
- Copy model files to Pi
- Compile model files on Pi
- Run a modified version of the demo script



