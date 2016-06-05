#pragma once
//
// Node
// 

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
    const std::vector<const Node*>& GetDependents() const { return _dependentNodes; }

    const NodeOutputBase& GetOutputHandle(size_t index) const { return *_outputs[index]; }    
    
    template <typename ValueType>
    std::vector<ValueType> GetOutputValue(size_t outputIndex) const;
    
protected:
    // The constructor for Node is kind of 
    Node(const std::vector<NodeInput *>& inputs, const std::vector<NodeOutputBase *>& outputs);

    virtual void ComputeOutput() const {}; // TODO
    void RegisterOutputs() const; // TODO
    
private:
    friend class DirectedGraph;
    void AddDependent(const Node* dependent) const;
    void AddDependents() const;

    static size_t _nextNodeId;
    NodeId _id;    
    std::vector<NodeInput *> _inputs;
    std::vector<NodeOutputBase *> _outputs;
    
    mutable std::vector<const Node *> _dependentNodes;    
};

#include "../tcc/Node.tcc"
