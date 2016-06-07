#pragma once
//
// Node
// 

#include "Port.h"
#include "OutputPort.h"
#include "UniqueId.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

class InputPort;

//
// Node: superclass for all node types. 
//
class Node
{
public:
    typedef UniqueId NodeId;
    
    // Returns the unique ID for this node
    const NodeId Id() const { return _id; }
    
    // Returns the type of this node (e.g., "Input", "Mean")
    virtual std::string GetTypeName() const = 0;

    // Returns the input "ports" for this node        
    const std::vector<InputPort*>& GetInputs() const { return _inputs; }
    
    // Convenience functions to get various properties of the outputs
    template <typename ValueType>
    std::vector<ValueType> GetOutputValue(size_t outputIndex) const;
    Port::OutputType GetOutputType(size_t outputIndex) const;
    size_t GetOutputSize(size_t outputIndex) const;

    // Get all nodes that depend (directly) on us        
    const std::vector<const Node*>& GetDependentNodes() const { return _dependentNodes; }
    
protected:
    // The constructor for Node is kind of gross. The arguments (and the _inputs and _outputs members)
    // should perhaps be vectors of references instead of pointers.
    Node(const std::vector<InputPort *>& inputs, const std::vector<OutputPortBase *>& outputs);

    // Compute is implemented by subclasses to calculate their result and set them in their outputs
    virtual void Compute() const {};
    void RegisterOutputs() const; // TODO
    
private:
    friend class DirectedGraph;
    void AddDependent(const Node* dependent) const;
    void AddDependencies() const;

    NodeId _id;

    std::vector<InputPort *> _inputs;
    std::vector<OutputPortBase *> _outputs;
    std::unordered_map<Port::PortId, OutputPortBase*> _outputs2;
    
    mutable std::vector<const Node *> _dependentNodes;    
};

#include "../tcc/Node.tcc"
