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

NodeEdge::OutputType Node::GetOutputType(size_t outputIndex) const
{
    return _outputs[outputIndex]->Type();    
}

size_t Node::GetOutputSize(size_t outputIndex) const
{
    return _outputs[outputIndex]->Size();
}

void Node::AddDependent(const Node* dependent) const
{
    _dependentNodes.push_back(dependent);
}

void Node::AddDependencies() const
{
    for(const auto& input: _inputs)
    {
        input->Node()->AddDependent(this);
    }
}
