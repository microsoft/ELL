## Compiling ELL Models

ELL provides a very cool neural net model compiler that can target a specific platform and produce optimized
code for that platform.  This helps to make your models run faster on low end hardware.

So we will take the models we loaded, either from [darknet](darknet.md) or [cntk](cntk.md) and compile them
for raspberry pi.