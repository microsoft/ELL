////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Node.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Node.h"
#include "InputPort.h"
#include "ModelTransformer.h"

/// <summary> model namespace </summary>
namespace model
{
    Node::Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs) : _inputs(inputs), _outputs(outputs), _id(NodeId())
    {};

    void Node::AddInputPort(InputPortBase* input)
    {
        _inputs.push_back(input);
    }

    void Node::MapAllOutputPorts(ModelTransformer& transformer, const std::shared_ptr<Node>& other) const
    {
        MapAllOutputPorts(transformer, other.get());
    }

    void Node::MapAllOutputPorts(ModelTransformer& transformer, Node* other) const
    {
        auto numOutputs = _outputs.size();
        assert(other->_outputs.size() == numOutputs);
        for(int index = 0; index < numOutputs; ++index)
        {
            transformer.MapPort(*_outputs[index], *(other->_outputs[index]));
        }
    }

    void Node::AddDependent(const Node* dependent) const 
    {
        _dependentNodes.push_back(dependent); 
    }

    void Node::RegisterDependencies() const
    {
        for (const auto& input : _inputs) 
        {
            for (const auto& node : input->GetInputNodes())
            {
                node->AddDependent(this);
            }
        }
    }
}
