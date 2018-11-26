////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReplaceSourceAndSinkNodesPass.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ReplaceSourceAndSinkNodesPass.h"

#include <model/include/InputNode.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputNode.h>

#include <model/optimizer/include/OptimizationPassRegistry.h>

#include <nodes/include/SinkNode.h>
#include <nodes/include/SourceNode.h>

#include <utilities/include/Exception.h>

namespace ell
{
namespace
{
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
        auto newNode = transformer.AddNode<model::InputNode<ValueType>>(shape);
        transformer.MapNodeOutput(thisNode->output, newNode->output);

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

        auto shape = thisNode->GetShape();
        const auto& newInput = transformer.GetCorrespondingInputs(thisNode->input);
        auto newNode = transformer.AddNode<model::OutputNode<ValueType>>(newInput, shape);

        transformer.MapNodeOutput(thisNode->output, newNode->output);
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
// ReplaceSourceAndSinkNodesPass methods
//
void ReplaceSourceAndSinkNodesPass::OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const
{
    ReplaceSourceOrSinkNode(node, context.GetTransformer());
}

void ReplaceSourceAndSinkNodesPass::AddToRegistry()
{
    model::OptimizationPassInfo info = {
        "ReplaceSourceAndSinkNodesPass",
        [](const model::ModelOptimizerOptions& settings) { return true; },
        [] { return std::make_unique<ReplaceSourceAndSinkNodesPass>(); }
    };
    model::OptimizationPassRegistry::AddPass(info);
}
} // namespace ell
