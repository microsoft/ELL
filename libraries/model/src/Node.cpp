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

// utilities
#include "Variant.h"
#include "ObjectDescription.h"
#include "ISerializable.h"

// stl
#include <unordered_set>

/// <summary> model namespace </summary>
namespace model
{
    Node::Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs) : _inputs(inputs), _outputs(outputs), _id(NodeId())
    {};

    void Node::AddInputPort(InputPortBase* input)
    {
        _inputs.push_back(input);
    }

    std::vector<const Node*> Node::GetParentNodes() const
    {
        std::unordered_set<const Node*> nodes;
        for (const auto& port : _inputs)
        {
            for (const auto& node : port->GetParentNodes())
            {
                nodes.insert(node);
            }
        }
        return std::vector<const Node*>{ nodes.begin(), nodes.end() };
    }

    void Node::AddDependent(const Node* dependent) const 
    {
        _dependentNodes.push_back(dependent); 
    }

    void Node::RegisterDependencies() const
    {
        for (const auto& input : _inputs) 
        {
            for (const auto& node : input->GetParentNodes())
            {
                node->AddDependent(this);
            }
            for (const auto& range : input->GetInputRanges())
            {
                range.ReferencedPort()->ReferencePort();
            }
        }
    }

    void Node::Refine(ModelTransformer& transformer) const
    {
        Copy(transformer);
    }

    utilities::ObjectDescription Node::GetDescription() const
    {
        auto result = utilities::ObjectDescription::FromType(*this);
        result.AddField("id", _id);
        if(_inputs.size() > 0)
        {
//            result.AddField("inputs", _inputs);
        }
        // TODO: do we just add a single field called inputs, and serialize the whole list of inputs?
        // then we need a way to serialize a vector of arbitrary (serializable) stuff
        // otherwise, we could add a separate field per input port and require the subclass to serialize its ports
        // maybe we can just add a helper function here to serialize an input port
        for (auto inputPort : _inputs)
        {
            auto ranges = inputPort->GetInputRanges();
        }

        return result;
    }

}
