////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleTransformations.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ExampleTransformations.h"

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputNode.h>

using namespace ell::model;

namespace
{
template <typename Container, typename Function>
auto Transform(const Container& container, Function fn)
{
    std::vector<decltype(fn(container[0]))> result;
    result.reserve(container.size());
    std::transform(container.begin(), container.end(), std::back_inserter(result), fn);
    return result;
}

bool IsOutputNode(const Node& node)
{
    return dynamic_cast<const OutputNodeBase*>(&node) != nullptr;
}

std::vector<const OutputPortBase*> GetReferencedPorts(const std::vector<const InputPortBase*>& inputs)
{
    return Transform(inputs, [](auto input) { return &input->GetReferencedPort(); });
}
} // namespace

// AddMetadataToOutputTransformation
Submodel AddMetadataToOutputTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
{
    // Rewrite tree, copying all nodes but adding metadata to any OutputNodes
    auto onto = GetReferencedPorts(submodel.GetInputPorts());
    auto result = transformer.TransformSubmodelOnto(submodel, onto, context, [this](const Node& node, ModelTransformer& transformer) {
        if (IsOutputNode(node))
        {
            CopyOutputNode(dynamic_cast<const OutputNodeBase*>(&node), transformer);
        }
        else
        {
            transformer.CopyNode(node);
        }
    });

    return result;
}

void AddMetadataToOutputTransformation::CopyOutputNode(const OutputNodeBase* node, ModelTransformer& transformer) const
{
    switch (node->GetOutputPort().GetType())
    {
    case Port::PortType::smallReal:
        CopyOutputNode<float>(dynamic_cast<const OutputNode<float>*>(node), transformer);
        break;
    case Port::PortType::real:
        CopyOutputNode<double>(dynamic_cast<const OutputNode<double>*>(node), transformer);
        break;
    case Port::PortType::integer:
        CopyOutputNode<int32_t>(dynamic_cast<const OutputNode<int32_t>*>(node), transformer);
        break;
    case Port::PortType::bigInt:
        CopyOutputNode<int64_t>(dynamic_cast<const OutputNode<int64_t>*>(node), transformer);
        break;
    default:
        throw 0;
    }
}

template <typename T>
void AddMetadataToOutputTransformation::CopyOutputNode(const OutputNode<T>* node, ModelTransformer& transformer) const
{
    const auto& newInput = transformer.GetCorrespondingInputs(node->input);
    auto newNode = transformer.AddNode<OutputNode<T>>(newInput);
    newNode->GetMetadata().SetEntry("a", "b");
    transformer.MapNodeOutput(node->output, newNode->output);
}

// CombineNodesTransformation
Submodel CombineNodesTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
{
    // Look for 2 consecutive nodes with metadata with the key "a", and replace them with a single node with the key "b"
    auto onto = GetReferencedPorts(submodel.GetInputPorts());
    auto result = transformer.TransformSubmodelOnto(submodel, onto, context, [this](const Node& node, ModelTransformer& transformer) {
        if (ShouldReplaceNode(node))
        {
            ReplaceNodeAndParent(node, transformer);
        }
        else
        {
            transformer.CopyNode(node);
        }
    });

    return result;
}

bool CombineNodesTransformation::ShouldReplaceNode(const Node& node) const
{
    auto parents = node.GetParentNodes();
    if (parents.size() != 1)
    {
        return false;
    }

    auto parent = parents[0];
    if (!(IsOutputNode(node) && IsOutputNode(*parent) && node.GetOutputPort(0)->GetType() == parent->GetOutputPort(0)->GetType()))
    {
        return false;
    }

    return node.GetMetadata().HasEntry("a") && parent->GetMetadata().HasEntry("a");
}

void CombineNodesTransformation::ReplaceNodeAndParent(const Node& node, ModelTransformer& transformer) const
{
    auto parent = static_cast<const OutputNodeBase*>(node.GetParentNodes()[0]);
    ReplaceNodeAndParent(static_cast<const OutputNodeBase*>(&node), parent, transformer);
}

void CombineNodesTransformation::ReplaceNodeAndParent(const OutputNodeBase* node, const OutputNodeBase* parent, ModelTransformer& transformer) const
{
    switch (node->GetOutputPort().GetType())
    {
    case Port::PortType::smallReal:
        ReplaceNodeAndParent<float>(dynamic_cast<const OutputNode<float>*>(node), dynamic_cast<const OutputNode<float>*>(parent), transformer);
        break;
    case Port::PortType::real:
        ReplaceNodeAndParent<double>(dynamic_cast<const OutputNode<double>*>(node), dynamic_cast<const OutputNode<double>*>(parent), transformer);
        break;
    case Port::PortType::integer:
        ReplaceNodeAndParent<int32_t>(dynamic_cast<const OutputNode<int32_t>*>(node), dynamic_cast<const OutputNode<int32_t>*>(parent), transformer);
        break;
    case Port::PortType::bigInt:
        ReplaceNodeAndParent<int64_t>(dynamic_cast<const OutputNode<int64_t>*>(node), dynamic_cast<const OutputNode<int64_t>*>(parent), transformer);
        break;
    default:
        throw 0;
    }
}

template <typename T>
void CombineNodesTransformation::ReplaceNodeAndParent(const OutputNode<T>* node, const OutputNode<T>* parent, ModelTransformer& transformer) const
{
    const auto& newInput = transformer.GetCorrespondingInputs(parent->input);
    auto newNode = transformer.AddNode<OutputNode<T>>(newInput);
    newNode->GetMetadata().SetEntry("a", "b");
    transformer.MapNodeOutput(node->output, newNode->output);
}
