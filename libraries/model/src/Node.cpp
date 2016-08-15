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

    bool Node::Refine(ModelTransformer& transformer) const
    {
        _didRefine = true;
        RefineNode(transformer);
        return _didRefine;
    }

    void Node::RefineNode(ModelTransformer& transformer) const
    {
        _didRefine = false;
        Copy(transformer);
    }

    void Node::Serialize(utilities::Serializer& serializer) const
    {
        serializer.Serialize("id", _id);
    }

    void Node::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        ModelSerializationContext& newContext = dynamic_cast<ModelSerializationContext&>(context);
        NodeId oldId;
        serializer.Deserialize("id", oldId, context);
        newContext.MapNode(oldId, this);
    }
}
