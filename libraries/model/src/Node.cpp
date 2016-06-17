////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Node.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Node.h"
#include "InputPort.h"

/// <summary> model namespace </summary>
namespace model
{
    Node::Node(const std::vector<InputPort*>& inputs, const std::vector<OutputPortBase*>& outputs) : _inputs(inputs), _outputs(outputs), _id(UniqueId())
    {};

    void Node::AddInputPort(InputPort* input)
    {
        _inputs.push_back(input);
    }

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

    void Node::RegisterDependencies() const
    {
        for (const auto& input : _inputs) 
        {
            for (const auto& range : input->GetInputRanges())
            {
                range.referencedPort->Node()->AddDependent(this);
            }
        }
    }
}
