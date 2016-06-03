#pragma once
//
// Node
// 

#include "NodeOutput.h"

#include <string>
#include <memory>
#include <vector>

class Node
{
public:
    void AddDependent(const Node* dependent);

    const NodeOutputBase& GetOutputHandle(size_t index) const;

protected:
    Node(const std::vector<NodeOutputBase*> inputs, const std::vector<NodeOutputBase*>& outputs);
    void RegisterOutputs(); // ???

private:
    friend class DirectedGraph;

    std::vector<NodeInput> _inputs;
    std::vector<NodeOutputBase*> _outputs;
    
    std::vector<std::shared_ptr<Node>> _dependentNodes; // perhaps this is a std::set
};

#include "../tcc/Node.tcc"
