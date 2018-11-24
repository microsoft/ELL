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

namespace ell
{

using namespace model;
using namespace nodes;
using namespace utilities;
using namespace utilities::logging;

namespace passes
{
    struct OptimizeReorderDataNodes::State
    {
        template <typename ValueType>
        bool TryOptimizeReorderNode(const Node& nodeToOptimize, ModelTransformer& transformer)
        {
            if (std::find(nodesToIgnore.begin(), nodesToIgnore.end(), nodeToOptimize.GetId()) != nodesToIgnore.end())
            {
                Log() << "Previous seen ReorderDataNode [id = " << nodeToOptimize.GetId().ToString() << "] ignored" << EOL;
                return true;
            }

            if (auto reorderNode = dynamic_cast<const ReorderDataNode<ValueType>*>(&nodeToOptimize))
            {
                const auto& node = *reorderNode;

                Log() << "ReorderDataNode [id = " << node.GetId().ToString() << "] detected" << EOL;

                if (node.GetParentNodes().empty())
                {
                    Log() << "ReorderDataNode [id = " << node.GetId().ToString() << "] has no parents " << EOL;
                    return false;
                }

                auto inputLayout = node.GetInputMemoryLayout();
                auto outputLayout = node.GetOutputMemoryLayout();
                const OutputPort<ValueType>* finalOutputPort = nullptr;

                {
                    auto currentNode = &node;
                    while (currentNode != nullptr)
                    {
                        // iff we have one dependent node and it's a reorder node
                        const ReorderDataNode<ValueType>* nextNode = nullptr;
                        if (currentNode->GetDependentNodes().size() == 1 &&
                            (nextNode = dynamic_cast<const ReorderDataNode<ValueType>*>(currentNode->GetDependentNodes()[0])))
                        {
                            Log() << "Removing node ReorderDataNode [id = " << currentNode->GetId().ToString() << "] since it is followed by another ReorderDataNode" << EOL;

                            transformer.DeleteNode(*currentNode);
                        }
                        else
                        {
                            Log() << "ReorderDataNode [id = " << currentNode->GetId().ToString() << "] is a terminal node in this chain of ReorderDataNodes" << EOL;

                            // this is the final reorder node that we're going to optimize in this pass
                            // get its output layout
                            outputLayout = currentNode->GetOutputMemoryLayout();

                            // and its output port
                            finalOutputPort = &currentNode->output;
                        }

                        // add this node to the list of ignored nodes
                        nodesToIgnore.push_back(currentNode->GetId());

                        // move to the next node
                        currentNode = nextNode;
                    }
                }

                if (inputLayout == outputLayout)
                {
                    Log() << "ReorderDataNode chain's input and output memory layout are the same. Eligible for removal." << EOL;

                    // if the input and output layouts match, map the output of the parent node to this chain of
                    // reorder nodes to the end of this chain
                    auto parentOutput = static_cast<const OutputPort<ValueType>*>(node.input.GetInputElement(0).ReferencedPort());
                    const auto& correspondingParentOutput = transformer.GetCorrespondingOutputs(*parentOutput);
                    transformer.MapNodeOutput(*finalOutputPort, correspondingParentOutput);
                }
                else
                {
                    // otherwise, create a new reorder node and use the input to the chain and map its output to the
                    // final output of the chain
                    const auto& newInput = transformer.GetCorrespondingInputs(node.input);
                    auto newNode = transformer.AddNode<nodes::ReorderDataNode<ValueType>>(newInput, inputLayout, outputLayout, node.GetPaddingValue());
                    transformer.MapNodeOutput(*finalOutputPort, newNode->output);

                    Log() << "ReorderDataNode chain's input and output memory layout are different. Entire chain is being "
                             "replaced by a new node [id = "
                          << newNode->GetId() << "]" << EOL;
                }

                return true;
            }

            return false;
        }

        std::vector<model::Node::NodeId> nodesToIgnore;
    };

    OptimizeReorderDataNodes::OptimizeReorderDataNodes() :
        _state(new OptimizeReorderDataNodes::State)
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

        transformer.CopyNode(node);
    }

    void OptimizeReorderDataNodes::AddToRegistry()
    {
        model::OptimizationPassInfo info = {
            "OptimizeReorderDataNodes",
            [](const model::ModelOptimizerOptions& settings) { return settings.phase == model::OptimizerPhase::optimize; },
            []() { return std::make_unique<OptimizeReorderDataNodes>(); }
        };
        model::OptimizationPassRegistry::AddPass(info);
    }
} // namespace passes
} // namespace ell
