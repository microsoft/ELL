#pragma once
//
// Node
// 

#include "NodeEdge.h"
#include "NodeOutput.h"

#include <string>
#include <memory>
#include <vector>
#include <set>

class NodeInput;

//
// Node: superclass for all node types. 
//
class Node
{
public:
    typedef int NodeId;
    
    const NodeId Id() const { return _id; }
    virtual std::string NodeType() const = 0;
    
    const std::vector<NodeInput*>& GetInputs() const { return _inputs; }
    
    // Convenience functions to get properties of outputs
    template <typename ValueType>
    std::vector<ValueType> GetOutputValue(size_t outputIndex) const;
    NodeEdge::OutputType GetOutputType(size_t outputIndex) const;
    size_t GetOutputSize(size_t outputIndex) const;
        
    const std::vector<const Node*>& GetDependentNodes() const { return _dependentNodes; }
    
protected:
    // The constructor for Node is kind of gross. The arguments (and the _inputs and _outputs members)
    // should perhaps be vectors of references instead of pointers.
    Node(const std::vector<NodeInput *>& inputs, const std::vector<NodeOutputBase *>& outputs);

    // ComputeOutput is implemented by subclasses to calculate their result and set them in their outputs
    virtual void ComputeOutput() const {};
    void RegisterOutputs() const; // TODO
    
private:
    friend class DirectedGraph;
    void AddDependent(const Node* dependent) const;
    void AddDependencies() const;

    static size_t _nextNodeId;
    NodeId _id;    
    std::vector<NodeInput *> _inputs;
    std::vector<NodeOutputBase *> _outputs;
    
    mutable std::vector<const Node *> _dependentNodes;    
};

#include "../tcc/Node.tcc"
