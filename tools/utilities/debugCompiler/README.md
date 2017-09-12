## Compiler Debug Tool

This tool tests the results of the reference implementation and the compiled
implementation of a given model against a given test image.

It then compares the output of each NeuralNetworkLayer in the model and prints 
a report showing statistics of each layer (both compiled and reference).

It also writes out a .csv file for each layer containing two colums, the
reference column and the compiled column so you can see the actual numbers.

As a side effect it also converts the given test image to a raw data file.
For example, if you provided `coffeemug.jpg` you will see `coffeemug.jpg.dat`
which is the resized image, converted to double floating point values then dumped
as a raw array, so the file contains `double[n]` where n is the size of the file 
divided by 8.

The tool also outputs a `graph.dgml` file which you can load into Visual Studio to
view the compiled Node graph, with annotated Error properties on each node.  It also
contains a style that will map the error values to different levels of red so you 
can easily see where the trouble spots are.

**Usage**: debugCompiler [options]

    Input options
            --inputMapFile (-imap) []   Path to the input *.map file
            --inputTestFile (-itf) []   Path to the input test file containing image to process
    Output options
            --inputScale (-is) [1/255]  How to scale input values (default 1/255)
            --outputDirectory (-od) []  Location of output files (default cwd)
            --report [true]             Generate markdown report
            --graph [true]              Write DGML graph
    Code-generation options
            --optimize (-opt) [true]    Optimize output code
            --blas [false]              Emit code that calls BLAS
            --help (-h) [false]         Print help and exit

### Visualize Results

Once a report is generated you can also visualize the results using the
following tool that lives in the ELL/tools/utilities/compare/src folder.

    visualizeReport.py reportFileName

If you provide the report.md file in the path to your report output directory you will see a series of images showing layer by layer comparisons.


### Details

The way this tool works is to inject DebugSinkNodes between each NeuralNetworkLayer of the
ell::model::Node graph.  This DebugSinkNodes provide a call back mechanism so the 
tool can capture the actual outputs during execution of the model, and this works both
for the reference implemtation and the compiled implementation.

The compiled implementation is executed in process using the IRExecutionEngine jitter
which is nice because it means we don't have to emit C++ code, and compile it using 
cmake and a C++ compiler.

