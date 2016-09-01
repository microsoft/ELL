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
#include "ISerializable.h"
#include "ObjectDescription.h"

// stl
#include <unordered_set>

/// <summary> model namespace </summary>
namespace model
{
    Node::Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs) : _id(NodeId()), _inputs(inputs), _outputs(outputs)
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
            for (const auto& range : input->GetInputElements().GetRanges())
            {
                range.ReferencedPort()->ReferencePort();
            }
        }
    }

    void Node::InvokeCopy(ModelTransformer& transformer) const
    {
        Copy(transformer);
    }

    bool Node::InvokeRefine(ModelTransformer& transformer) const
    {
        return Refine(transformer);
    }

    // Default implementation of Refine just copies and returns false
    bool Node::Refine(ModelTransformer& transformer) const
    {
        Copy(transformer);
        return false;
    }

    void Node::AddProperties(utilities::ObjectDescription& description) const
    {
        description.SetType(*this);
        description["id"] << _id;
    }

    void Node::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        NodeId oldId;
        description["id"] >> oldId;
        ModelSerializationContext& newContext = dynamic_cast<ModelSerializationContext&>(context);
        newContext.MapNode(oldId, this);
    }
}
