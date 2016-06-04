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
    void AddDependent(const Node* dependent) const;

    const NodeOutputBase& GetOutputRef(size_t index) const { return *_outputs[index]; }
    // const NodeOutputBase& GetOutputRef(std::string name) const; // ...

protected:
    Node(const std::vector<NodeInputBase*>& inputs, const std::vector<NodeOutputBase*>& outputs);
    void RegisterOutputs(); 
    virtual void ComputeOutput() {};

private:
    friend class DirectedGraph;

    std::vector<NodeInputBase*> _inputs; //?
    std::vector<NodeOutputBase*> _outputs;
    
    mutable std::set<const Node*> _dependentNodes;
};

#include "../tcc/Node.tcc"
