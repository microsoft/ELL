////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Node.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Node.h"
#include "InputPort.h"

Node::Node(const std::vector<InputPort*>& inputs, const std::vector<OutputPortBase*>& outputs) : _inputs(inputs), _outputs(outputs), _id(UniqueId())
{
};

Port::PortType Node::GetOutputType(size_t outputIndex) const
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
