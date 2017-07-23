## Comparison Tool

This tool compares the results of the reference implementation and the compiled
implementation of a given model against a given test image.

It compares the output of each NeuralNetworkLayer in the model and prints 
a report showing statistics of each layer (both compiled and reference).

It also writes out a .csv file for each layer containing two colums, the
reference column and the compiled column so you can see the actual numbers.

As a side effect it also converts the given test image to a raw data file.
For example, if you provided `schoolbus.jpg` you will see `schoolbus.jpg.dat`
which is the resized image, converted to double floating point values then dumped
as a raw array, so the file contains `double[n]` where n is the size of the file 
divided by 8.

The tool also outputs a `graph.dgml` file which you can load into Visual Studio to
view the compiled Node graph, with annotated Error properties on each node.  It also
contains a style that will map the error values to different levels of red so you 
can easily see where the trouble spots are.

### Details

The way this tool works is to inject DebugSinkNodes between each NeuralNetworkLayer of the
ell::model::Node graph.  This DebugSinkNodes provide a call back mechanism so the compare
tool can capture the actual outputs during execution of the model, and this works both
for the reference implemtation and the compiled implementation.

The compiled implementation is executed in process using the IRExecutionEngine jitter.
