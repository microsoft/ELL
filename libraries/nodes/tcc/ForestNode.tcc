////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestNode.h"
#include "ConstantNode.h"
#include "MultiplexerNode.h"
#include "BinaryOperationNode.h"
#include "DemultiplexerNode.h"
#include "SingleElementThresholdNode.h"
#include "SumNode.h"

// stl
#include <vector>
#include <memory>

namespace nodes
{
    template<typename SplitRuleType, typename EdgePredictorType>
    ForestNode<SplitRuleType, EdgePredictorType>::ForestNode(const model::PortElements<double>& input, const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest) : Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _treeOutputs(this, treeOutputsPortName, forest.NumTrees()), _edgeIndicatorVector(this, edgeIndicatorVectorPortName, forest.NumEdges()), _forest(forest)
    {}

    template<typename SplitRuleType, typename EdgePredictorType>
    ForestNode<SplitRuleType, EdgePredictorType>::ForestNode() : Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _treeOutputs(this, treeOutputsPortName, 0), _edgeIndicatorVector(this, edgeIndicatorVectorPortName, 0)
    {}


    template<typename SplitRuleType, typename EdgePredictorType>
    utilities::ObjectDescription ForestNode<SplitRuleType, EdgePredictorType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node, ForestNode<SplitRuleType, EdgePredictorType>>("Linear predictor node");
        description.template AddProperty<decltype(_input)>("input", "Input port");
        description.template AddProperty<decltype(_output)>("output", "Output port");
        description.template AddProperty<decltype(_treeOutputs)>("treeOutputs", "The individual tree outputs");
        description.template AddProperty<decltype(_edgeIndicatorVector)>("edgeIndicatorVector", "The edge indicator vector");
        description.template AddProperty<decltype(_forest)>("forest", "The forest");
        return description;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    utilities::ObjectDescription ForestNode<SplitRuleType, EdgePredictorType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node, ForestNode<SplitRuleType, EdgePredictorType>>();
        description["input"] = _input;
        description["output"] = _output;
        description["treeOutputs"] = _treeOutputs;
        description["edgeIndicatorVector"] = _edgeIndicatorVector;
        description["forest"] = _forest;
        return description;
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    void ForestNode<SplitRuleType, EdgePredictorType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description["input"] >> _input;
        description["output"] >> _output;
        description["treeOutputs"] >> _treeOutputs;
        description["edgeIndicatorVector"] >> _edgeIndicatorVector;
        description["forest"] >> _forest;
    }
    
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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ForestNode<SplitRuleType, EdgePredictorType>>(newPortElements, _forest);
        transformer.MapNodeOutput(output, newNode->output);
        transformer.MapNodeOutput(treeOutputs, newNode->treeOutputs);
        transformer.MapNodeOutput(edgeIndicatorVector, newNode->edgeIndicatorVector);
    }

    template<typename SplitRuleType, typename EdgePredictorType>
    bool ForestNode<SplitRuleType, EdgePredictorType>::Refine(model::ModelTransformer & transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());      
        const auto& interiorNodes = _forest.GetInteriorNodes();

        // create a place to store references to the output ports of the sub-models at each interior node
        std::vector<model::PortElements<bool>> interiorNodeSplitIndicators(interiorNodes.size());
        std::vector<model::PortElements<double>> interiorNodeSubModels(interiorNodes.size());
        
        // visit interior nodes bottom-up (in reverse topological order)
        for(int nodeIndex = static_cast<int>(interiorNodes.size())-1; nodeIndex >= 0; --nodeIndex) // Note: index var must be signed or else end condition is never met
        {            
            const auto& edges = interiorNodes[nodeIndex].GetOutgoingEdges();

            // get the sub-model that represents each outgoing edge
            model::PortElements<double> edgeOutputs;
            for(size_t j = 0; j < edges.size(); ++j)
            {
                const auto& edgePredictor = edges[j].GetPredictor();
                auto edgePredictorNode = AddNodeToModelTransformer(newPortElements, edgePredictor, transformer);

                if(edges[j].IsTargetInterior()) // target node is itself an interior node: reverse topological order guarantees that it's already visited
                {
                    model::PortElements<double> elements = interiorNodeSubModels[edges[j].GetTargetNodeIndex()];

                    auto sumNode = transformer.AddNode<BinaryOperationNode<double>>(edgePredictorNode->output, elements, BinaryOperationNode<double>::OperationType::add);
                    edgeOutputs.Append(sumNode->output);
                }
                else // target node is a leaf
                {
                    edgeOutputs.Append(edgePredictorNode->output);
                }
            }

            // add the sub-model that computes the split rule
            auto splitRuleNode = AddNodeToModelTransformer(newPortElements, interiorNodes[nodeIndex].GetSplitRule(), transformer);
            interiorNodeSplitIndicators[nodeIndex] = {splitRuleNode->output};
            
            // ...and selects the output value
            auto selectorNode = transformer.AddNode<MultiplexerNode<double, bool>>(edgeOutputs, splitRuleNode->output);
            interiorNodeSubModels[nodeIndex] = {selectorNode->output};
        }

        // Now compute the edge indicator vector
        auto trueNode = transformer.AddNode<ConstantNode<bool>>(true); // the constant 'true'
        std::vector<model::PortElements<bool>> edgeIndicatorSubModels(_forest.NumEdges());

        // Vector with index of the incoming edge for each internal node (with sentinel value of -1 for tree roots)
        std::vector<int> incomingEdgeIndices(interiorNodes.size(), -1);
        for(size_t nodeIndex = 0; nodeIndex < interiorNodes.size(); ++nodeIndex)
        {
            auto parentEdgeIndex = incomingEdgeIndices[nodeIndex];
            auto isRoot = parentEdgeIndex == -1;
            const auto& edgeSelector = interiorNodeSplitIndicators[nodeIndex];
            const auto& node = interiorNodes[nodeIndex];
            const auto& childEdges = node.GetOutgoingEdges();
            auto numChildren = childEdges.size();
            model::PortElements<bool> parentIndicator = isRoot ? trueNode->output : edgeIndicatorSubModels[parentEdgeIndex];

            // The Demultiplexer node computes the indicator value for all the children at once, by copying its input value (a '1' if it's the root)
            // to the selected child.
            auto muxNode = transformer.AddNode<DemultiplexerNode<bool, bool>>(parentIndicator, edgeSelector, numChildren);
            for(size_t edgePosition = 0; edgePosition < numChildren; ++edgePosition)
            {
                auto edgeIndex = node.GetFirstEdgeIndex() + edgePosition;
                model::PortElements<bool> childOut = {muxNode->output, edgePosition};
                edgeIndicatorSubModels[edgeIndex] = childOut;

                // If this edge's target node has an outgoing edge, record ourself as its parent
                if(childEdges[edgePosition].IsTargetInterior())
                {
                    auto childNode = childEdges[edgePosition].GetTargetNodeIndex();
                    incomingEdgeIndices[childNode] = static_cast<int>(edgeIndex);
                }
            }
        }
        // collect the individual entries for the indicator vector into a single PortElements object
        model::PortElements<bool> edgeIndicatorVectorElements(edgeIndicatorSubModels);

        // collect the sub-models that represent the trees of the forest
        model::PortElements<double> treeSubModels;
        for(size_t rootIndex : _forest.GetRootIndices())
        {
            treeSubModels.Append(interiorNodeSubModels[rootIndex]);
        }

        // make a copy and add the bias term
        auto treesPlusBias = treeSubModels;
        auto biasNode = transformer.AddNode<ConstantNode<double>>(_forest.GetBias());
        treesPlusBias.Append(biasNode->output);

        // sum all of the trees
        auto sumNode = transformer.AddNode<SumNode<double>>(treesPlusBias);

        // Map all the outputs from the original node to the refined model outputs         
        transformer.MapNodeOutput(output, sumNode->output);
        transformer.MapNodeOutput(treeOutputs, treeSubModels);
        transformer.MapNodeOutput(edgeIndicatorVector, edgeIndicatorVectorElements);
        return true;
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