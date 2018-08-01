////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeReorderDataNodes.cpp (passes)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizeReorderDataNodes.h"

// model
#include "ModelTransformer.h"
#include "OptimizationPassRegistry.h"

// nodes
#include "ReorderDataNode.h"

// utilities
#include "Exception.h"
#include "Logger.h"

using namespace ell::utilities::logging;

namespace ell
{
namespace passes
{
    namespace
    {
        template <typename ValueType>
        void MergeNodeOutputWithParents(const nodes::ReorderDataNode<ValueType>& reorderNode, model::ModelTransformer& transformer)
        {
            auto& inputRanges = reorderNode.input.GetInputElements().GetRanges();
            auto parentOutputPort = static_cast<const model::OutputPort<ValueType>*>(inputRanges[0].ReferencedPort());

            auto correspondingParentOutput = transformer.GetCorrespondingOutputs(*parentOutputPort).GetElement(0).ReferencedPort();
            transformer.MapNodeOutput(reorderNode.output, *correspondingParentOutput);
        }
    }
    struct OptimizeReorderDataNodes::State
    {
        template <typename ValueType>
        bool TryOptimizeReorderNode(const model::Node& node, model::ModelTransformer& transformer)
        {
            if (std::find(nodesToIgnore.begin(), nodesToIgnore.end(), node.GetId()) != nodesToIgnore.end())
            {
                MergeNodeOutputWithParents(*dynamic_cast<const nodes::ReorderDataNode<ValueType>*>(&node), transformer);
                return true;
            }

            if (auto reorderNode = dynamic_cast<const nodes::ReorderDataNode<ValueType>*>(&node))
            {
                Log() << "ReorderDataNode detected" << EOL;

                if (!reorderNode->input.GetInputElements().IsFullPortOutput())
                {
                    return false;
                }

                auto inputMemoryLayout = reorderNode->GetInputMemoryLayout();
                auto outputMemoryLayout = reorderNode->GetOutputMemoryLayout();

                auto dependentReorderNode = reorderNode;
                while ((dependentReorderNode = dynamic_cast<const nodes::ReorderDataNode<ValueType>*>(dependentReorderNode->GetDependentNodes()[0])))
                {
                    outputMemoryLayout = dependentReorderNode->GetOutputMemoryLayout();

                    nodesToIgnore.push_back(dependentReorderNode->GetId());
                }

                if (inputMemoryLayout == outputMemoryLayout)
                {
                    Log() << "Reorder node's input and output memory layout are the same. Eligible for optimization." << EOL;

                    MergeNodeOutputWithParents(*reorderNode, transformer);
                }
                else
                {
                    auto newInput = transformer.TransformPortElements(reorderNode->input.GetPortElements());
                    auto newNode = transformer.AddNode<nodes::ReorderDataNode<ValueType>>(newInput, inputMemoryLayout, outputMemoryLayout, reorderNode->GetPaddingValue());
                    transformer.MapNodeOutput(reorderNode->output, newNode->output);
                }

                return true;
            }

            return false;
        }

        std::vector<model::Node::NodeId> nodesToIgnore;
    };

    OptimizeReorderDataNodes::OptimizeReorderDataNodes()
        : _state(new OptimizeReorderDataNodes::State)
    {
    }

    OptimizeReorderDataNodes::~OptimizeReorderDataNodes() = default;

    void OptimizeReorderDataNodes::OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const
    {
        auto& transformer = context.GetTransformer();

        if (_state->TryOptimizeReorderNode<float>(node, transformer))
        {
            return;
        }
        if (_state->TryOptimizeReorderNode<double>(node, transformer))
        {
            return;
        }

        node.Copy(transformer);
    }

    void OptimizeReorderDataNodes::AddToRegistry()
    {
        model::OptimizationPassInfo info = {
            "OptimizeReorderDataNodes",
            [](const model::ModelOptimizerOptions&) { return true; },
            []() { return std::make_unique<OptimizeReorderDataNodes>(); }
        };
        model::OptimizationPassRegistry::AddPass(info);
    }
}
}
