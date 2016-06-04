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
    
    virtual std::string Type() const = 0;
            
protected:
    Node(const std::vector<NodeInputBase*>& inputs, const std::vector<NodeOutputBase*>& outputs);
    virtual void ComputeOutput() {};
    void RegisterOutputs() const;
    
private:
    friend class DirectedGraph;
    void AddDependent(const Node* dependent) const;
    void AddDependents() const;

    std::vector<NodeInputBase*> _inputs;
    std::vector<NodeOutputBase*> _outputs;
    
    mutable std::set<const Node*> _dependentNodes;    
};

#include "../tcc/Node.tcc"
