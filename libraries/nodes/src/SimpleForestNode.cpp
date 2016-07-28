////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleForestNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleForestNode.h"
#include "ConstantNode.h"
#include "ElementSelectorNode.h"
#include "BinaryOperationNode.h"
#include "SumNode.h"

// stl
#include <vector>
#include <memory>

namespace nodes
{
    SimpleForestNode::SimpleForestNode(const model::OutputPortElements<double>& input, const predictors::SimpleForestPredictor& forest) : Node({ &_input }, { &_prediction, &_treeOutputs, &_edgeIndicatorVector }), _input(this, input, inputPortName), _prediction(this, outputPortName, 1), _treeOutputs(this, treeOutputsPortName, forest.NumTrees()), _edgeIndicatorVector(this, edgeIndicatorVectorPortName, forest.NumEdges()), _forest(forest)
    {}

    std::string SimpleForestNode::GetRuntimeTypeName() const
    {
        return "SimpleForestNode";
    }

    void SimpleForestNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<SimpleForestNode>(newOutputPortElements, _forest);
        transformer.MapOutputPort(prediction, newNode->prediction);
        transformer.MapOutputPort(treeOutputs, newNode->treeOutputs);
        transformer.MapOutputPort(edgeIndicatorVector, newNode->edgeIndicatorVector);
    }

    void SimpleForestNode::Refine(model::ModelTransformer & transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newOutputs = BuildSubModel(_forest, transformer.GetModel(), newOutputPortElements);
        transformer.MapOutputPort(prediction, newOutputs.prediction);
        //       transformer.MapOutputPort(treeOutputs, newOutputs.treeOutputs);
        //       transformer.MapOutputPort(edgeIndicatorVector, newOutputs.edgeIndicatorVector);
    }

    void SimpleForestNode::Compute() const
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

    SimpleForestSubModelOutputs BuildSubModel(const predictors::SimpleForestPredictor& forest, model::Model& model, const model::OutputPortElements<double>& outputPortElements)
    {
        const auto& interiorNodes = forest.GetInteriorNodes();
        std::vector<model::OutputPortRange> interiorNodeOutputs(interiorNodes.size());

        for(size_t i = interiorNodes.size(); i > 0; --i)
        {
            const auto& node = interiorNodes[i-1];
            const auto& edges = node.GetOutgoingEdges();

            model::OutputPortElements<double> edgeOutputs;
            for(size_t j = 0; j < edges.size(); ++j)
            {
                const auto& edgePredictor = edges[j].GetPredictor();
                auto edgePredictorNode = model.AddNode<ConstantNode<double>>(13);

                if(edges[j].IsTargetInterior()) // has subtree
                {
                    model::OutputPortElements<double> elements;
                    elements.AddRange(interiorNodeOutputs[edges[j].GetTargetNodeIndex()]);

                    auto sumNode = model.AddNode<BinaryOperationNode<double>>(edgePredictorNode->output, elements, BinaryOperationNode<double>::OperationType::add);
                    edgeOutputs.AddRange(sumNode->output);
                }
                else // leaf
                {
                    edgeOutputs.AddRange(edgePredictorNode->output);
                }
            }

            auto splitRuleNode = model.AddNode<ConstantNode<int>>(0);
            auto selectorNode = model.AddNode<ElementSelectorNode<double>>(edgeOutputs, splitRuleNode->output);
            interiorNodeOutputs[i-1] = selectorNode->output;
        }

        model::OutputPortElements<double> treeOutputs;
        for(size_t rootIndex : forest.GetTreeRootIndices()) // TODO: remove the word 'Tree' from this func name, and other members of that class
        {
            treeOutputs.AddRange(interiorNodeOutputs[rootIndex]);
        }

        auto sumNode = model.AddNode<SumNode<double>>(treeOutputs);
        
        return { 
            sumNode->output
        //,newNode->treeOutputs, newNode->edgeIndicatorVector
        };
    }
}