## Interactive / JIT profile Tool

The profile tool can be used to help spot bottlenecks in the compiled code for a 
model. It compiles the model and runs it (using the JIT on the host machine), gathering
timing information during the run. Then it writes a report summarizing the total time
to evaluate the model, the time spent in each node, and a summary of the time spent
per node type.

### Options

We have found that the evaluation times for a model can vary quite a bit. In particular,
the first few evaluations of a model produce wildly different run times. There are 2 options 
to counter this effect: `burnIn` and `numIterations`. The `numIterations` option specifies 
how many iterations to run while collecting timing data. The summary will then report the 
total and average time per iteration. The `burnIn` option is specifically meant to address
the problem with the first few iterations taking longer than later ones. If set, The `burnIn` 
option specifies the number of model evaluations to compute before starting the `numIterations`
evaluations that are measured.

### Usage

Help text for other options:

```
        --inputMapFilename (-imap) []    Path to the input map file
        --inputModelFilename (-imf) []   Path to the input model file
        --modelInputs (-in) []           Model inputs to use
        --modelOutputs (-out) []         Model outputs to use
        --defaultInputSize (-d) [1]      Default size of input node
        --testFile (-tf) []              Path to the test data (an image file)
        --outputFilename (-of) [<cout>]  File for profiling output ('<cout>' for stdout, blank or '<null>' for no output)
        --timingOutput []                File for node timing detail output ('<cout>' for stdout, blank or '<null>' for no output)
        --format (-fmt) [text]           Format for profiling output ('text' or 'json')  {text | json}
        --comment []                     Comment to embed in output
        --filter [true]                  Filter trivial nodes (InputNode and ConstantNode) from note type output
        --numIterations (-n) [1]         Number of times to run model during the profiling phase
        --burnIn [0]                     Number of initial iterations to run before starting the profiling phase
        --summary [false]                Print timing summary only
        --optimize [true]                Optimize compiled code
        --blas [true]                    Use BLAS libraries in compiled code
        --foldLinearOps [true]           Fold sequences of linear operations with constant coefficients into a single operation
        --vectorize (-vec) [false]       Enable ELL's vectorization
        --vectorWidth (-vw) [4]          Size of vector units
        --help (-h) [false]              Print help and exit
```

### Sample output

Text format
```
Node statistics
Node[1323]:	ConstantNode<float>                        	time: 0.00000 ms	count: 3
...
Node[1395]:	SoftmaxLayerNode<float>                    	time: 0.04053 ms	count: 3
Node[1396]:	OutputNode<float>                          	time: 0.00171 ms	count: 3


Node type statistics
OutputNode<float>                          	time: 0.00171 ms 	count: 3
...
ReorderDataNode<float>                     	time: 16.41016 ms 	count: 27
ReshapeImageNode<float>                    	time: 17.34546 ms 	count: 24
MatrixMatrixMultiplyNode<float>            	time: 29.07690 ms 	count: 24

Model statistics
Total time: 75.11304 ms 	count: 3	 time per run: 25.03768 ms
```

JSON format
```
{
"node_statistics": [
  {
    "name": "1323",
    "type": "ConstantNode<float>",
    "total_time": 0,
    "average_time": 0,
    "count": 3
  },

...

  {
    "name": "1395",
    "type": "SoftmaxLayerNode<float>",
    "total_time": 0.0273438,
    "average_time": 0.00911458,
    "count": 3
  },
  {
    "name": "1396",
    "type": "OutputNode<float>",
    "total_time": 0.000976562,
    "average_time": 0.000325521,
    "count": 3
  }
],
"node_type_statistics": [
  [  {
    "type": "InputNode<float>",
    "total_time": 0,
    "average_time": 0,
    "count": 3
  },

...


  {
    "type": "ReorderDataNode<float>",
    "total_time": 15.8164,
    "average_time": 0.585793,
    "count": 27
  },
  {
    "type": "ReshapeImageNode<float>",
    "total_time": 16.9109,
    "average_time": 0.70462,
    "count": 24
  },
  {
    "type": "MatrixMatrixMultiplyNode<float>",
    "total_time": 34.2175,
    "average_time": 1.42573,
    "count": 24
  }
],
"model_statistics": {
  "total_time": 78.0952,
  "average_time": 26.0317,
  "count": 3
}}
```

## Compiled profile tool

There is another profile tool that generates binary profiling applications to run on a target machine. You generate a project to compile on the target machine like this:

```
Windows: bin\make_profiler.cmd <model_filename> [<output_directory>] <profile options...>
Linux/Mac: bin/make_profiler.sh <model_filename> [<output_directory>] <profile options...>
```

then copy the resulting directory to the target machine, run CMake, and build the project.
