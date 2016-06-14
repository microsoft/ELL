Graph library
=============


Node
----
A `Node` represents a unit of computation. You could think of it as a function. It takes some number
of inputs and returns a number of outputs (imagine returning a struct of outputs). Individual inputs and
outputs of a node are called a `Port`.

In order to compute values and traverse the graph, we need to keep references to where our inputs come
from, and who depends on us. Inputs are indicated with an `InputPort`, which has a reference to the specific port
from another `Node` it consumes from. 

Port
----
An `Port` represents a singly-typed input or output of a `Node`. For instance, an FFT node might have 2 output ports, one for the magnitude
and one for the phase. Similarly, a pairwise addition node might have 2 input ports and one output port. The `InputPort` subclass keeps a
reference to the `OutputPort` it gets is output from. An `OutputPort` contains a cached value of the output to be used for computing the
graph output.

Graph
-----
The basic graph class (here, `Model`) would be the primary API point for interacting with the graph of nodes. The user-facing
`Compute`-type operations would be done on the graph object. 

Usage
-----

Here's a quick bogus scenario:

I've got an accelerometer that outputs a 3-dimensional signal, and I was to run some classifier on
its outputs. The awesome set of features and ML algos I came up with was:
- Filter the input to remove noise
- Take the instantaneous magnitude (2-norm) of the filtered (x,y,z) vector
- Take the mean over a 16-sample window of the magnitude signal
- Take the 16-sample FFT of the magnitude signal
- Feed the mean and amplitude of the FFT output into a classifier
- Yay! done

See the image "HighLevelGraph" ![HighLevelGraph](file://HighLevelGraph.png) for a terrible drawing of 
the high-level graph of the above series of computations.

## *Aside* On the equivalence between the graph, its serialization, and a functional code-like description of the computation
  
For the featurization library, I found it somewhat useful to have a simple serialized format that looked like

    <feature id> <feature type> <input 1> <input 2> ... <param 1> <param 2> ...

Using a modified version of that format, the pictured graph would look like:

    in     Input       3
    signal Filter      in     <filter params>
    mag    Magnitude   signal 16
    mean   Mean        mag    16
    fft    FFT         mag    16
    out    LinearClassifier    mean    fft.amplitude

... but look what happens when you add a little bit of punctuation:

    in = Input(3);
    signal = Filter(in, <filter params>);
    mag = Magnitude(signal, 16);
    mean = Mean(mag, 16);
    fft = FFT(mag, 16);
    out = LinearClassifier(mean, fft.amplitude);

Golly, it looks like code. If the runtime had functions implementing the node types, that would indeed be pretty close to the code.

IR representation
-----------------

I drew a quick graph showing what a lower-level IR-type representation might look like (though I punted
on decomposing the "Filter" node). It's the file IRGraph1.png ![IRGraph](file://IRGraph1.png). I'm sure
it's not obvious what the hell that picture is supposed to represent: I'll describe it later if anyone cares.

FullImage.png has both graphs side-by-side. for yuks.


