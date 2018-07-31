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
        bool TryOptimizeReorderNode(const model::Node& node, model::ModelTransformer& transformer)
        {
            if (auto reorderNode = dynamic_cast<const nodes::ReorderDataNode<ValueType>*>(&node))
            {
                Log() << "ReorderDataNode detected" << EOL;
                if (reorderNode->GetInputMemoryLayout() == reorderNode->GetOutputMemoryLayout())
                {
                    Log() << "Reorder node's input and output memory layout are the same. Eligible for optimziaton." << EOL;

                    auto& inputRanges = reorderNode->input.GetInputElements().GetRanges();
                    auto parentOutputPort = static_cast<const model::OutputPort<ValueType>*>(inputRanges[0].ReferencedPort());

                    auto correspondingParentOutput = transformer.GetCorrespondingOutputs(*parentOutputPort).GetElement(0).ReferencedPort();
                    transformer.MapNodeOutput(reorderNode->output, *correspondingParentOutput);

                    return true;
                }
            }

            return false;
        }

    }

    void OptimizeReorderDataNodes::OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const
    {
        auto& transformer = context.GetTransformer();

        if (TryOptimizeReorderNode<float>(node, transformer))
        {
            return;
        }
        if (TryOptimizeReorderNode<double>(node, transformer))
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
