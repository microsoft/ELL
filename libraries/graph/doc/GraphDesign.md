Graph library
=============

The idea with this representation was to make things as simple as possible without sacrificing 
functionality. It's a very simple standard Graph representation, with a small wrinkly in that nodes may have multiple outputs. 
There is also type information for the edges, but that's minor.

Node
----
A `Node` represents a piece of computation. You could think of it as a function. It takes some number
of inputs and returns a number of outputs (imagine returning a struct of outputs). Just to give them a 
name, I'm calling an individual output (a field in the struct) a `Port`.

In order to compute values and traverse the graph, we need to keep references to where our inputs come
from, and who depends on us. Inputs are indicated with an `Edge`, which indicates the specific part
of another `Node`'s output it consumes from. For these "downstream" dependencies, we don't (I don't think) 
need to store the particular `Port`, but just a reference/pointer to a `Node`

Edge
----
An `Edge` is a reference to the values needed for a `Node`'s input. One representation would just be a (`Node`, `Port`) 
pair. 

Graph
-----
The basic graph class (here, `DirectedGraph`) would be the primary API point for interacting with the graph of nodes. The user-facing
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

*Aside*: On the equivalence between the graph, its serialization, and a functional code-like description of the computation
  
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

IR representation
-----------------

I drew a quick graph showing what a lower-level IR-type representation might look like (though I punted
on decomposing the "Filter" node). It's the file IRGraph1.png ![IRGraph](file://IRGraph1.png). I'm sure
it's not obvious what the hell that picture is supposed to represent: I'll describe it later if anyone cares.

FullImage.png has both graphs side-by-side. for yuks.


Silly reference implementation
------------------------------

I hacked up some quick classes just to see that these thoughts can at least compile. Here are the class definitions I wrote:

###Node###

    class Node
    {
    public:
        // Q: Do we want to have 2 kinds of ports: input and output (or a flag saying what direction a port points?)
        class Port
        {        
        public:
            typedef int Id;
            enum class Type { Real, Boolean, Categorical };
            size_t GetSize();
            Type GetType();
            
        private:
            int _size;
            Type _type;
        };
        
        Node(const std::vector<Edge>& inputs);

        template <typename OutputType>
        OutputType ComputeOutput(Port::Id portId) const;
            
        const std::vector<Edge>& GetInputEdges() const;
        void AddDependent(const std::shared_ptr<Node>& dependent);

    private:
        // vector of inputs
        std::vector<Edge> _inputs;
            
        // vector of outputs  
        std::vector<Port> _outputs;

        // dependents
        std::vector<std::shared_ptr<Node>> _dependents;

        // Other attributes go here
    };



###Edge###

    class Edge
    {
    public:
        const std::shared_ptr<Node>& GetNode() const;
        Node::Port::Id GetPort();
        
    private:
        std::shared_ptr<Node> _destinationNode;
        Node::Port::Id _destinationPort;
    };


###Graph###

    class DirectedGraph
    {
    public:    

        // Factory method for creating nodes
        template <NodeType, Args...>
        std::shared_ptr<NodeType> CreateNode(const std::vector<shared_ptr<Node>>& inputs, args... args);
        
        template <typename Visitor>
        void Visit(Visitor& visitor) const; // Visits all nodes in the graph

        template <typename Visitor>
        void Visit(const NodeRef outputNode, Visitor& visitor) const; // Visits all nodes in the graph necssary to compute outputNode

        template <typename Visitor>
        void Visit(const std::vector<NodeRef>& outputNode, Visitor& visitor) const; // Visits all nodes in the graph necssary to compute all outputNodes
        
    private:
        // the nodes (?)
        // maybe an unordered_map<NodeIdType, shared_ptr<Node>> for lookup
    };
