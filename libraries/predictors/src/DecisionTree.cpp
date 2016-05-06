////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DecisionTree.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DecisionTree.h"

// layers
#include "Coordinatewise.h"
#include "DecisionTreePath.h"
#include "Sum.h"

// stl
#include <stdexcept>
#include <cassert>

namespace predictors
{
    DecisionTree::Node::Node(double outputValue) : _outputValue(outputValue)
    {}

    double DecisionTree::Node::getOutputValue() const
    {
        return _outputValue;
    }

    bool DecisionTree::Node::IsLeaf() const
    {
        if (_interiorNode == nullptr)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    double DecisionTree::Node::Predict(const dataset::IDataVector& dataVector) const
    {
        double output = _outputValue;
        if (_interiorNode != nullptr)
        {
            output += _interiorNode->Predict(dataVector);
        }
        return output;
    }

    DecisionTree::InteriorNode& DecisionTree::Node::Split(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue)
    {
        // confirm that this is a leaf
        assert(IsLeaf());

        _interiorNode = std::make_unique<InteriorNode>(std::move(splitRule), Node(negativeEdgeOutputValue), Node(positiveEdgeOutputValue));
        return *_interiorNode;
    }

    DecisionTree::InteriorNode::InteriorNode(SplitRule splitRule, Node negativeChild, Node positiveChild) :
        _splitRule(splitRule), _negativeChild(std::move(negativeChild)), _positiveChild(std::move(positiveChild))
    {}

    const DecisionTree::SplitRule & DecisionTree::InteriorNode::GetSplitRule() const
    {
        return _splitRule;
    }

    DecisionTree::Node & DecisionTree::InteriorNode::GetNegativeChild()
    {
        return _negativeChild;
    }

    const DecisionTree::Node & DecisionTree::InteriorNode::GetNegativeChild() const
    {
        return _negativeChild;
    }

    DecisionTree::Node & DecisionTree::InteriorNode::GetPositiveChild()
    {
        return _positiveChild;
    }

    const DecisionTree::Node & DecisionTree::InteriorNode::GetPositiveChild() const
    {
        return _positiveChild;
    }

    uint64_t DecisionTree::InteriorNode::NumInteriorNodesInSubtree() const
    {
        uint64_t num = 1;

        if(_negativeChild._interiorNode != nullptr)
        {
            num += _negativeChild._interiorNode->NumInteriorNodesInSubtree();
        }

        if(_positiveChild._interiorNode != nullptr)
        {
            num += _positiveChild._interiorNode->NumInteriorNodesInSubtree();
        }

        return num;
    }

    double DecisionTree::InteriorNode::Predict(const dataset::IDataVector& dataVector) const
    {
        if (dataVector[_splitRule.featureIndex] <= _splitRule.threshold)
        {
            return _negativeChild.Predict(dataVector);
        }
        else
        {
            return _positiveChild.Predict(dataVector);
        }
    }

    DecisionTree::DecisionTree(double rootOutputValue) : _root(rootOutputValue)
    {}

    uint64_t DecisionTree::NumInteriorNodes() const
    {
        if(_root._interiorNode == nullptr)
        {
            return 0;
        }
        return _root._interiorNode->NumInteriorNodesInSubtree();
    }

    DecisionTree::Node& DecisionTree::GetRoot()
    {
        return _root;
    }

    double DecisionTree::Predict(const dataset::IDataVector & dataVector) const
    {
        return 0.0; // TODO
    }

    void DecisionTree::AddToModel(layers::Model & model, layers::CoordinateList inputCoordinates) const
    {
        FlatTree flatTree;
        BuildFlatTree(flatTree, inputCoordinates, _root._interiorNode.get());

        auto thresholdsLayer = std::make_unique<layers::Coordinatewise>(std::move(flatTree.negativeThresholds), std::move(flatTree.splitRuleCoordinates), layers::Coordinatewise::OperationType::add);
        auto thresholdsLayerCoordinates = model.AddLayer(std::move(thresholdsLayer));

        auto decisionTreePathLayer = std::make_unique<layers::DecisionTreePath>(std::move(flatTree.edgeToInteriorNode), std::move(thresholdsLayerCoordinates));
        auto decisionTreePathLayerCoordinates = model.AddLayer(std::move(decisionTreePathLayer));

        auto nonRootOutputValuesLayer = std::make_unique<layers::Coordinatewise>(std::move(flatTree.nonRootOutputValues), std::move(decisionTreePathLayerCoordinates), layers::Coordinatewise::OperationType::multiply);
        auto nonRootOutputValuesLayerCoordinates = model.AddLayer(std::move(nonRootOutputValuesLayer));

        auto sumLayer = std::make_unique<layers::Sum>(std::move(nonRootOutputValuesLayerCoordinates));
        auto sumLayerCoordinates = model.AddLayer(std::move(sumLayer));

        auto biasLayer = std::make_unique<layers::Coordinatewise>(layers::Coordinatewise::OperationType::add);
        biasLayer->Append(_root._outputValue, sumLayerCoordinates[0]);
        model.AddLayer(std::move(biasLayer));
    }

    void predictors::DecisionTree::BuildFlatTree(FlatTree& flatTree, const layers::CoordinateList& inputCoordinates, InteriorNode* interiorNodePtr) const
    {
        if (interiorNodePtr == nullptr)
        {
            return;
        }

        const auto& splitRule = interiorNodePtr->GetSplitRule();
        auto splitRuleCoordinate = inputCoordinates[splitRule.featureIndex];
        auto negativeThreshold = -(splitRule.threshold);

        flatTree.splitRuleCoordinates.AddCoordinate(splitRuleCoordinate);
        flatTree.negativeThresholds.push_back(negativeThreshold);

        flatTree.nonRootOutputValues.push_back(interiorNodePtr->GetNegativeChild()._outputValue);
        flatTree.nonRootOutputValues.push_back(interiorNodePtr->GetPositiveChild()._outputValue);

        if (interiorNodePtr->GetNegativeChild().IsLeaf())
        {
            flatTree.edgeToInteriorNode.push_back(0);
            flatTree.edgeToInteriorNode.push_back(0);
        }
        else
        {
            auto negativeChildIndex = flatTree.splitRuleCoordinates.Size();
            flatTree.edgeToInteriorNode.push_back(negativeChildIndex);

            // as a placeholder, set edgeToInteriorNode[positiveEdgeIndex] = 0
            auto positiveEdgeIndex = flatTree.edgeToInteriorNode.size();
            flatTree.edgeToInteriorNode.push_back(0);

            // recurse (DFS) to the negative side
            BuildFlatTree(flatTree, inputCoordinates, interiorNodePtr->GetNegativeChild()._interiorNode.get());
            
            // set edgeToInteriorNode[positiveEdgeIndex] to its correct value
            auto positiveChildIndex = flatTree.splitRuleCoordinates.Size();
            flatTree.edgeToInteriorNode[positiveEdgeIndex] = positiveChildIndex;
            
            // recurse (DFS) to the positive side
            BuildFlatTree(flatTree, inputCoordinates, interiorNodePtr->GetPositiveChild()._interiorNode.get());
        }
    }
}
