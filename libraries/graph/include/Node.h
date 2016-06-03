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
    void AddDependent(const Node* dependent);

    const NodeOutputBase& GetOutput(size_t index) const;
    const NodeOutputBase& GetOutput(std::string name) const;


protected:
    Node(const std::vector<NodeOutputBase*> inputs, const std::vector<NodeOutputBase*>& outputs);
    void RegisterOutputs(); 

private:
    friend class DirectedGraph;

    std::vector<NodeInput> _inputs;
    std::vector<NodeOutputBase*> _outputs;
    
    std::set<std::shared_ptr<Node>> _dependentNodes;
};

#include "../tcc/Node.tcc"
