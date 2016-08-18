////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestNode.h"
#include "ConstantNode.h"
#include "ElementSelectorNode.h"
#include "BinaryOperationNode.h"
#include "SingleElementThresholdNode.h"
#include "SumNode.h"

// stl
#include <vector>
#include <memory>

namespace nodes
{
    template<typename SplitRuleType, typename EdgePredictorType>
    ForestNode<SplitRuleType, EdgePredictorType>::ForestNode(const model::OutputPortElements<double>& input, const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest) : Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _treeOutputs(this, treeOutputsPortName, forest.NumTrees()), _edgeIndicatorVector(this, edgeIndicatorVectorPortName, forest.NumEdges()), _forest(forest)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestNode<SplitRuleType, EdgePredictorType>::ForestNode() : Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _treeOutputs(this, treeOutputsPortName, 0), _edgeIndicatorVector(this, edgeIndicatorVectorPortName, 0)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
        serializer.Serialize("treeOutputs", _treeOutputs);
        serializer.Serialize("edgeIndicatorVector", _edgeIndicatorVector);
        serializer.Serialize("forest", _forest);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
        serializer.Deserialize("treeOutputs", _treeOutputs, context);
        serializer.Deserialize("edgeIndicatorVector", _edgeIndicatorVector, context);
        serializer.Deserialize("forest", _forest, context);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<ForestNode<SplitRuleType, EdgePredictorType>>(newOutputPortElements, _forest);
        transformer.MapOutputPort(output, newNode->output);
        transformer.MapOutputPort(treeOutputs, newNode->treeOutputs);
        transformer.MapOutputPort(edgeIndicatorVector, newNode->edgeIndicatorVector);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Refine(model::ModelTransformer & transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());      
        const auto& interiorNodes = _forest.GetInteriorNodes();

        // create a place to store references to the output ports of the sub-models at each interior node
        // TODO: waiting for OutputPortElements changes: this should be a vector of OutputPortElements and not OutputPortRange -it is currently a vector of Ranges because I need to AddRange them
        std::vector<model::OutputPortRange> interiorNodeSubModels(interiorNodes.size());

        // visit interior nodes bottom-up (in reverse topological order)
        for(size_t i = interiorNodes.size(); i > 0; --i)
        {
            const auto& edges = interiorNodes[i - 1].GetOutgoingEdges();

            // get the sub-model that represents each outgoing edge
            // TODO: waiting for OutputPortElements changes: why is this an OutputPortElements, while interiorNodeSubModels is a vector?
            model::OutputPortElements<double> edgeOutputs;
            for(size_t j = 0; j < edges.size(); ++j)
            {
                const auto& edgePredictor = edges[j].GetPredictor();
                auto edgePredictorNode = AddNodeToModelTransformer(newOutputPortElements, edgePredictor, transformer);

                if(edges[j].IsTargetInterior()) // target node is itself an interior node: reverse topological order guarantees that it's already visited
                {
                    // TODO: waiting for OutputPortElements changes: the following 3 lines should be one clean line - there are currently 3 because interiorNodeSubModels had to be an array of Ranges
                    model::OutputPortElements<double> elements;
                    auto targetNodeSubModelOutputs = interiorNodeSubModels[edges[j].GetTargetNodeIndex()];
                    elements.AddRange(targetNodeSubModelOutputs);

                    auto sumNode = transformer.AddNode<BinaryOperationNode<double>>(edgePredictorNode->output, elements, BinaryOperationNode<double>::OperationType::add);
                    edgeOutputs.AddRange(sumNode->output);
                }
                else // target node is a leaf
                {
                    edgeOutputs.AddRange(edgePredictorNode->output);
                }
            }

            // add the sub-model that computes the split rule
            auto splitRuleNode = AddNodeToModelTransformer(newOutputPortElements, interiorNodes[i - 1].GetSplitRule(), transformer);

            auto selectorNode = transformer.AddNode<ElementSelectorNode<double, bool>>(edgeOutputs, splitRuleNode->output);
            interiorNodeSubModels[i-1] = selectorNode->output;
        }

        // collect the sub-models that represent the trees of the forest
        // TODO: waiting for OutputPortElements changes: why is this an OutputPortElements, while interiorNodeSubModels is a vector?
        model::OutputPortElements<double> treeSubModels;
        for(size_t rootIndex : _forest.GetRootIndices())
        {
            treeSubModels.AddRange(interiorNodeSubModels[rootIndex]);
        }

        // add the bias term
        auto biasNode = transformer.AddNode<ConstantNode<double>>(_forest.GetBias());
        treeSubModels.AddRange(biasNode->output);

        // sum all of the trees
        auto sumNode = transformer.AddNode<SumNode<double>>(treeSubModels);
        
        transformer.MapOutputPort(output, sumNode->output);
        // TODO: waiting for OutputPortElements changes:
        //       transformer.MapOutputPort(treeOutputs, ...);
        //       transformer.MapOutputPort(edgeIndicatorVector, ...);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Compute() const
    {
        // forest output
        _output.SetOutput({ _forest.Predict(_input) });

        // individual tree outputs
        std::vector<double> treeOutputs(_forest.NumTrees());
        for(size_t i=0; i<_forest.NumTrees(); ++i)
        {
            treeOutputs[i] = _forest.Predict(_input, _forest.GetRootIndex(i));
        }
        _treeOutputs.SetOutput(std::move(treeOutputs));

        // path indicator
        auto edgeIndicator = _forest.GetEdgeIndicatorVector(_input);
        _edgeIndicatorVector.SetOutput(std::move(edgeIndicator));
    }
}