//
// Node.cpp
//

#include "Node.h"

Node::Node(const std::vector<NodeInputBase*>& inputs, const std::vector<NodeOutputBase*>& outputs) : _inputs(inputs), _outputs(outputs)
{
};

void Node::AddDependent(const Node* dependent) const
{
    _dependentNodes.insert(dependent);
}

void Node::AddDependents() const
{
    for(const auto& input: _inputs)
    {
        input->GetNode()->AddDependent(this);
    }
}
