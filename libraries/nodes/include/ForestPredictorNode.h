////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestPredictorNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "DemultiplexerNode.h"
#include "ForestPredictorNode.h"
#include "MultiplexerNode.h"
#include "SingleElementThresholdNode.h"
#include "SumNode.h"

#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <predictors/include/ForestPredictor.h>
#include <predictors/include/SingleElementThresholdPredictor.h>

#include <memory>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> Implements a forest node, which wraps the forest predictor. </summary>
    ///
    /// <typeparam name="SplitRuleType"> The split rule type. </typeparam>
    /// <typeparam name="EdgePredictorType"> The edge predictor type. </typeparam>
    template <typename SplitRuleType, typename EdgePredictorType>
    class ForestPredictorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* treeOutputsPortName = "treeOutputs";
        static constexpr const char* edgeIndicatorVectorPortName = "edgeIndicatorVector";
        const model::InputPort<double>& input = _input;
        const model::OutputPort<double>& output = _output;
        const model::OutputPort<double>& treeOutputs = _treeOutputs;
        const model::OutputPort<bool>& edgeIndicatorVector = _edgeIndicatorVector;
        /// @}

        using ForestPredictor = predictors::ForestPredictor<SplitRuleType, EdgePredictorType>;

        /// <summary> Default Constructor </summary>
        ForestPredictorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The predictor's input. </param>
        /// <param name="forest"> The forest predictor. </param>
        ForestPredictorNode(const model::OutputPort<double>& input, const ForestPredictor& forest);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<SplitRuleType, EdgePredictorType>("ForestPredictorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<double> _input;

        // Outputs
        model::OutputPort<double> _output;
        model::OutputPort<double> _treeOutputs;
        model::OutputPort<bool> _edgeIndicatorVector;

        // Forest
        ForestPredictor _forest;
    };

    /// <summary> Defines an alias representing a simple forest node, which holds a forest with a SingleElementThresholdPredictor as the split rule and ConstantPredictors on the edges. </summary>
    using SimpleForestPredictorNode = ForestPredictorNode<predictors::SingleElementThresholdPredictor, predictors::ConstantPredictor>;

    /// <summary> Convenience function to add a forest predictor node. </summary>
    ///
    /// <param name="input"> The predictor's input. </param>
    /// <param name="forest"> The forest predictor. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename SplitRuleType, typename EdgePredictorType>
    const model::OutputPort<double>& ForestPredictor(const model::OutputPort<double>& input,
                                                     const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest);

} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename SplitRuleType, typename EdgePredictorType>
    ForestPredictorNode<SplitRuleType, EdgePredictorType>::ForestPredictorNode(const model::OutputPort<double>& input, const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest) :
        Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _treeOutputs(this, treeOutputsPortName, forest.NumTrees()),
        _edgeIndicatorVector(this, edgeIndicatorVectorPortName, forest.NumEdges()),
        _forest(forest)
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    ForestPredictorNode<SplitRuleType, EdgePredictorType>::ForestPredictorNode() :
        Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _treeOutputs(this, treeOutputsPortName, 0),
        _edgeIndicatorVector(this, edgeIndicatorVectorPortName, 0)
    {
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictorNode<SplitRuleType, EdgePredictorType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["forest"] << _forest;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictorNode<SplitRuleType, EdgePredictorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["forest"] >> _forest;

        _treeOutputs.SetSize(_forest.NumTrees());
        _edgeIndicatorVector.SetSize(_forest.NumEdges());
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictorNode<SplitRuleType, EdgePredictorType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<ForestPredictorNode<SplitRuleType, EdgePredictorType>>(newPortElements, _forest);
        transformer.MapNodeOutput(output, newNode->output);
        transformer.MapNodeOutput(treeOutputs, newNode->treeOutputs);
        transformer.MapNodeOutput(edgeIndicatorVector, newNode->edgeIndicatorVector);
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    bool ForestPredictorNode<SplitRuleType, EdgePredictorType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        const auto& interiorNodes = _forest.GetInteriorNodes();

        // create a place to store references to the output ports of the sub-models at each interior node
        std::vector<model::PortElements<bool>> interiorNodeSplitIndicators(interiorNodes.size());
        std::vector<model::PortElements<double>> interiorNodeSubModels(interiorNodes.size());

        // visit interior nodes bottom-up (in reverse topological order)
        for (int nodeIndex = static_cast<int>(interiorNodes.size()) - 1; nodeIndex >= 0; --nodeIndex) // Note: index var must be signed or else end condition is never met
        {
            const auto& edges = interiorNodes[nodeIndex].GetOutgoingEdges();

            // get the sub-model that represents each outgoing edge
            model::PortElements<double> edgeOutputs;
            for (size_t j = 0; j < edges.size(); ++j)
            {
                const auto& edgePredictor = edges[j].GetPredictor();
                auto edgePredictorNode = AddNodeToModelTransformer(newPortElements, edgePredictor, transformer);

                if (edges[j].IsTargetInterior()) // target node is itself an interior node: reverse topological order guarantees that it's already visited
                {
                    model::PortElements<double> elements = interiorNodeSubModels[edges[j].GetTargetNodeIndex()];

                    const auto& edgeSum = Add(edgePredictorNode->output, transformer.SimplifyOutputs(elements));
                    edgeOutputs.Append(edgeSum);
                }
                else // target node is a leaf
                {
                    edgeOutputs.Append(edgePredictorNode->output);
                }
            }

            // add the sub-model that computes the split rule
            auto splitRuleNode = AddNodeToModelTransformer(newPortElements, interiorNodes[nodeIndex].GetSplitRule(), transformer);
            interiorNodeSplitIndicators[nodeIndex] = { splitRuleNode->output };

            // ...and selects the output value
            auto selectorNode = transformer.AddNode<MultiplexerNode<double, bool>>(edgeOutputs, splitRuleNode->output);
            interiorNodeSubModels[nodeIndex] = { selectorNode->output };
        }

        // Now compute the edge indicator vector
        const auto& trueValue = Constant(transformer, true); // the constant 'true'
        std::vector<model::PortElements<bool>> edgeIndicatorSubModels(_forest.NumEdges());

        // Vector with index of the incoming edge for each internal node (with sentinel value of -1 for tree roots)
        std::vector<int> incomingEdgeIndices(interiorNodes.size(), -1);
        for (size_t nodeIndex = 0; nodeIndex < interiorNodes.size(); ++nodeIndex)
        {
            auto parentEdgeIndex = incomingEdgeIndices[nodeIndex];
            auto isRoot = parentEdgeIndex == -1;
            const auto& edgeSelector = interiorNodeSplitIndicators[nodeIndex];
            const auto& node = interiorNodes[nodeIndex];
            const auto& childEdges = node.GetOutgoingEdges();
            auto numChildren = childEdges.size();
            model::PortElements<bool> parentIndicator = isRoot ? trueValue : edgeIndicatorSubModels[parentEdgeIndex];

            // The Demultiplexer node computes the indicator value for all the children at once, by copying its input value (a '1' if it's the root)
            // to the selected child.
            auto muxNode = transformer.AddNode<DemultiplexerNode<bool, bool>>(parentIndicator, edgeSelector, numChildren);
            for (size_t edgePosition = 0; edgePosition < numChildren; ++edgePosition)
            {
                auto edgeIndex = node.GetFirstEdgeIndex() + edgePosition;
                model::PortElements<bool> childOut = { muxNode->output, edgePosition };
                edgeIndicatorSubModels[edgeIndex] = childOut;

                // If this edge's target node has an outgoing edge, record ourself as its parent
                if (childEdges[edgePosition].IsTargetInterior())
                {
                    auto childNode = childEdges[edgePosition].GetTargetNodeIndex();
                    incomingEdgeIndices[childNode] = static_cast<int>(edgeIndex);
                }
            }
        }
        // collect the individual entries for the indicator vector into a single PortElements object
        model::PortElements<bool> edgeIndicatorVectorElements(edgeIndicatorSubModels);
        const auto& edgeIndicatorVectorElementsOutput = transformer.SimplifyOutputs(edgeIndicatorVectorElements);

        // collect the sub-models that represent the trees of the forest
        model::PortElements<double> treeSubModels;
        for (size_t rootIndex : _forest.GetRootIndices())
        {
            treeSubModels.Append(interiorNodeSubModels[rootIndex]);
        }
        const auto& treeSubModelsOutput = transformer.SimplifyOutputs(treeSubModels);

        // Make a copy and add the bias term
        auto treesPlusBias = treeSubModels;
        const auto& bias = Constant(transformer, _forest.GetBias());
        treesPlusBias.Append(bias);

        // Sum all of the trees
        const auto& treeSum = Sum(transformer.SimplifyOutputs(treesPlusBias));

        // Map all the outputs from the original node to the refined model outputs
        transformer.MapNodeOutput(output, treeSum);
        transformer.MapNodeOutput(treeOutputs, treeSubModelsOutput);
        transformer.MapNodeOutput(edgeIndicatorVector, edgeIndicatorVectorElementsOutput);
        return true;
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    void ForestPredictorNode<SplitRuleType, EdgePredictorType>::Compute() const
    {
        // forest output
        auto inputDataVector = typename ForestPredictor::DataVectorType(_input.GetValue());
        _output.SetOutput({ _forest.Predict(inputDataVector) });

        // individual tree outputs
        std::vector<double> treeOutputs(_forest.NumTrees());
        for (size_t i = 0; i < _forest.NumTrees(); ++i)
        {
            treeOutputs[i] = _forest.Predict(inputDataVector, _forest.GetRootIndex(i));
        }
        _treeOutputs.SetOutput(std::move(treeOutputs));

        // path indicator
        auto edgeIndicator = _forest.GetEdgeIndicatorVector(inputDataVector);
        _edgeIndicatorVector.SetOutput(std::move(edgeIndicator));
    }

    template <typename SplitRuleType, typename EdgePredictorType>
    const model::OutputPort<double>& ForestPredictor(const model::OutputPort<double>& input,
                                                     const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }

        auto node = model->AddNode<ForestPredictorNode<SplitRuleType, EdgePredictorType>>(input, forest);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
