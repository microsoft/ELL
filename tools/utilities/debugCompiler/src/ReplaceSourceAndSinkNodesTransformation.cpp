////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReplaceSourceAndSinkNodesTransformation.cpp (debugCompiler)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ReplaceSourceAndSinkNodesTransformation.h"

#include <model/include/InputNode.h>
#include <model/include/InputPort.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputNode.h>
#include <model/include/OutputPort.h>

#include <nodes/include/SinkNode.h>
#include <nodes/include/SourceNode.h>

#include <utilities/include/Exception.h>

#include <vector>

namespace ell
{
namespace
{
    template <typename Container, typename Function>
    auto Transform(const Container& container, Function fn)
    {
        return utilities::TransformVector(container.begin(), container.end(), fn);
    }

    std::vector<const model::OutputPortBase*> GetReferencedPorts(const std::vector<const model::InputPortBase*>& inputs)
    {
        return Transform(inputs, [](auto input) { return &input->GetReferencedPort(); });
    }

    // returns 'true' if we handled the situation, else 'false'. If we return 'false', keep trying other ValueTypes.
    template <typename ValueType>
    bool TryReplaceSourceNode(const model::Node& node, model::ModelTransformer& transformer)
    {
        auto thisNode = dynamic_cast<const nodes::SourceNode<ValueType>*>(&node);
        if (thisNode == nullptr)
        {
            return false;
        }

        auto shape = thisNode->GetShape();
        const auto& newOutput = model::Input<ValueType>(transformer, shape);
        transformer.MapNodeOutput(thisNode->output, newOutput);

        return true;
    }

    template <typename ValueType>
    bool TryReplaceSinkNode(const model::Node& node, model::ModelTransformer& transformer)
    {
        auto thisNode = dynamic_cast<const nodes::SinkNode<ValueType>*>(&node);
        if (thisNode == nullptr)
        {
            return false;
        }

        const auto& newInput = transformer.GetCorrespondingInputs(thisNode->input);
        const auto& newOutput = model::Output(newInput);

        transformer.MapNodeOutput(thisNode->output, newOutput);
        return true;
    }

    void ReplaceSourceOrSinkNode(const model::Node& node, model::ModelTransformer& transformer)
    {
        if (TryReplaceSourceNode<float>(node, transformer))
        {
            return;
        }
        if (TryReplaceSourceNode<double>(node, transformer))
        {
            return;
        }

        if (TryReplaceSinkNode<float>(node, transformer))
        {
            return;
        }
        if (TryReplaceSinkNode<double>(node, transformer))
        {
            return;
        }

        transformer.CopyNode(node);
    }
} // namespace

//
// ReplaceSourceAndSinkNodesTransformation methods
//
model::Submodel ReplaceSourceAndSinkNodesTransformation::Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const model::TransformContext& context) const
{
    // in-place transformation
    auto onto = transformer.GetCorrespondingOutputs(GetReferencedPorts(submodel.GetInputs()));
    model::Model destModel = submodel.GetModel().ShallowCopy();
    auto result = transformer.TransformSubmodelOnto(submodel, destModel, onto, context, [](const model::Node& node, model::ModelTransformer& transformer) {
        ReplaceSourceOrSinkNode(node, transformer);
    });
    return result;
}
} // namespace ell
