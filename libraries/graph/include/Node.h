#pragma once

#include <vector>
#include <memory>

class Edge;
class Node
{
public:
    // Q: Do we want to have 2 kinds of ports: input and output (or a flag saying what direction a port points?)
    class Port
    {        
    public:
        enum class Type { Real, Boolean, Categorical };
        size_t GetSize();
        Type GetType();
        typedef int Id;
        
    private:
        int _size;
        Type _type;
    };
    
    Node();
    Node(const std::vector<Edge>& inputs);

    template <typename OutputType>
    OutputType ComputeOutput(Port::Id portId) const;
        
    const std::vector<Edge>& GetInputEdges() const { return _inputs; }
    void AddDependent(const std::shared_ptr<Node>& dependent);

private:
    // vector of inputs
    std::vector<Edge> _inputs;
    std::vector<Port> _outputs;
        
    // vector of outputs  
    std::vector<std::shared_ptr<Node>> _dependents;

    // Other attributes (perhaps a template parameter)
    // string ID
    // string[] metadata
    // ???
};




#include "../tcc/Node.tcc"
