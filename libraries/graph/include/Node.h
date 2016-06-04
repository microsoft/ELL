#pragma once
//
// Node
// 

#include "NodeOutput.h"
#include "NodeInput.h"

#include <string>
#include <memory>
#include <vector>
#include <set>

class Node
{
public:
    typedef int NodeId;
    
    const NodeId Id() const { return _id; }
    virtual std::string Type() const = 0;
    
    const std::vector<NodeInput*>& GetInputs() const { return _inputs; }
    const std::vector<const Node*>& GetDependents() const { return _dependentNodes; }

    const NodeOutputBase& GetOutput(size_t index) const { return *_outputs[index]; }    
    
protected:
    Node(const std::vector<NodeInput*>& inputs, const std::vector<NodeOutputBase*>& outputs);

    virtual void ComputeOutput() const {}; // TODO
    void RegisterOutputs() const; // TODO
    
private:
    friend class DirectedGraph;
    void AddDependent(const Node* dependent) const;
    void AddDependents() const;

    static size_t _nextNodeId;
    NodeId _id;    
    std::vector<NodeInput*> _inputs;
    std::vector<NodeOutputBase*> _outputs;
    
    mutable std::vector<const Node*> _dependentNodes;    
};

#include "../tcc/Node.tcc"
