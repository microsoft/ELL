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
#include "SumNode.h"
#include "SubModelBuilders.h"

// stl
#include <vector>
#include <memory>

namespace nodes
{
    template<typename SplitRuleType, typename EdgePredictorType>
    ForestNode<SplitRuleType, EdgePredictorType>::ForestNode(const model::OutputPortElements<double>& input, const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest) : Node({ &_input }, { &_prediction, &_treeOutputs, &_edgeIndicatorVector }), _input(this, input, inputPortName), _prediction(this, outputPortName, 1), _treeOutputs(this, treeOutputsPortName, forest.NumTrees()), _edgeIndicatorVector(this, edgeIndicatorVectorPortName, forest.NumEdges()), _forest(forest)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<ForestNode<SplitRuleType, EdgePredictorType>>(newOutputPortElements, _forest);
        transformer.MapOutputPort(prediction, newNode->prediction);
        transformer.MapOutputPort(treeOutputs, newNode->treeOutputs);
        transformer.MapOutputPort(edgeIndicatorVector, newNode->edgeIndicatorVector);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Refine(model::ModelTransformer & transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newOutputs = BuildSubModel(_forest, transformer.GetModel(), newOutputPortElements);
        transformer.MapOutputPort(prediction, newOutputs.prediction);
        // TODO: we need the transformer to support OutputPortElements
        //       transformer.MapOutputPort(treeOutputs, newOutputs.treeOutputs);
        //       transformer.MapOutputPort(edgeIndicatorVector, newOutputs.edgeIndicatorVector);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::Compute() const
    {
        // forest output
        _prediction.SetOutput({ _forest.Predict(_input) });

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

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestSubModelOutputs BuildSubModel(const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest, model::Model& model, const model::OutputPortElements<double>& outputPortElements) // call the last argument "inputs" or something like that
    {
        const auto& interiorNodes = forest.GetInteriorNodes();

        // create a place to store references to the output ports of the sub-models at each interior node
        // TODO: this should be a vector of OutputPortElements and not OutputPortRange -it is currently a vector of Ranges because I need to AddRange them
        std::vector<model::OutputPortRange> interiorNodeSubModels(interiorNodes.size());

        // visit interior nodes bottom-up (in reverse topological order)
        for(size_t i = interiorNodes.size(); i > 0; --i)
        {
            const auto& edges = interiorNodes[i - 1].GetOutgoingEdges();

            // get the sub-model that represents each outgoing edge
            // TODO: why is this an OutputPortElements, while interiorNodeSubModels is a vector?
            model::OutputPortElements<double> edgeOutputs;
            for(size_t j = 0; j < edges.size(); ++j)
            {
                const auto& edgePredictor = edges[j].GetPredictor();
                auto edgePredictorSubModel = BuildSubModel(edgePredictor, model, outputPortElements);

                if(edges[j].IsTargetInterior()) // target node is itself an interior node: reverse topological order guarantees that it's already visited
                {
                    // TODO: the following 3 lines should be one clean line - there are currently 3 because interiorNodeSubModels had to be an array of Ranges
                    model::OutputPortElements<double> elements;
                    auto targetNodeSubModelOutputs = interiorNodeSubModels[edges[j].GetTargetNodeIndex()];
                    elements.AddRange(targetNodeSubModelOutputs);

                    auto sumNode = model.AddNode<BinaryOperationNode<double>>(edgePredictorSubModel.output, elements, BinaryOperationNode<double>::OperationType::add);
                    edgeOutputs.AddRange(sumNode->output);
                }
                else // target node is a leaf
                {
                    edgeOutputs.AddRange(edgePredictorSubModel.output);
                }
            }

            // add the sub-model that computes the split rule
            auto splitRuleSubModel = BuildSubModel(interiorNodes[i - 1].GetSplitRule(), model, outputPortElements);
            auto selectorNode = model.AddNode<ElementSelectorNode<double, bool>>(edgeOutputs, splitRuleSubModel.output);
            interiorNodeSubModels[i-1] = selectorNode->output;
        }

        // collect the sub-models that represent the trees of the forest
        // TODO: why is this an OutputPortElements, while interiorNodeSubModels is a vector?
        model::OutputPortElements<double> treeSubModels;
        for(size_t rootIndex : forest.GetRootIndices())
        {
            treeSubModels.AddRange(interiorNodeSubModels[rootIndex]);
        }

        // add the bias term
        auto biasNode = model.AddNode<ConstantNode<double>>(forest.GetBias());
        treeSubModels.AddRange(biasNode->output);

        // sum all of the trees
        auto sumNode = model.AddNode<SumNode<double>>(treeSubModels);
        
        return { 
            sumNode->output
        //,newNode->treeOutputs, newNode->edgeIndicatorVector
        };
    }
}