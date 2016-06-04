//
// Node.cpp
//

#include "Node.h"

Node::Node(const std::vector<NodeInputBase*>& inputs, const std::vector<NodeOutputBase*>& outputs) : _inputs(inputs), _outputs(outputs)
{
    for(const auto& input: inputs)
    {
        input->GetNode()->AddDependent(this);
    }
};

void Node::AddDependent(const Node* dependent) const
{
    _dependentNodes.insert(dependent);
}