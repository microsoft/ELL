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
//    void GetOutputHandle(std::string name);  // ???

protected:
    Node(const std::vector<NodeOutputBase*> inputs, const std::vector<NodeOutputBase*>& outputs);
    void RegisterOutputs(); // ???

private:
    friend class DirectedGraph;
    std::vector<NodeOutputBase*> _inputs;
    std::vector<NodeOutputBase*> _outputs;
    std::vector<std::shared_ptr<Node>> _dependents;
};

#include "../tcc/Node.tcc"
