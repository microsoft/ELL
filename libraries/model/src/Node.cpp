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
}
