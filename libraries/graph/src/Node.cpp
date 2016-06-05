//
// Node.cpp
//

#include "Node.h"
#include "NodeInput.h"

size_t Node::_nextNodeId = 0;

Node::Node(const std::vector<NodeInput*>& inputs, const std::vector<NodeOutputBase*>& outputs) : _inputs(inputs), _outputs(outputs)
{
    _id = _nextNodeId; // Warning: now node creation isn't threadsafe
    ++_nextNodeId;
};

void Node::AddDependent(const Node* dependent) const
{
    _dependentNodes.push_back(dependent);
}

void Node::AddDependents() const
{
    for(const auto& input: _inputs)
    {
        input->GetNode()->AddDependent(this);
    }
}
