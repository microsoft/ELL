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
    bool DecisionTree::Child::IsLeaf() const
    {
        if (_node == nullptr)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    DecisionTree::InteriorNode& DecisionTree::Child::Split(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue)
    {
        // confirm that this is a leaf
        assert(IsLeaf());

        _node = std::make_unique<InteriorNode>(std::move(splitRule), negativeEdgeOutputValue, positiveEdgeOutputValue);
        return *_node;
    }

    DecisionTree::InteriorNode::InteriorNode(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue) : 
        _splitRule(splitRule), _negativeEdgeOutputValue(negativeEdgeOutputValue), _positiveEdgeOutputValue(positiveEdgeOutputValue)
    {}

    const DecisionTree::SplitRule & DecisionTree::InteriorNode::GetSplitRule() const
    {
        return _splitRule;
    }

    DecisionTree::Child & DecisionTree::InteriorNode::GetNegativeChild()
    {
        return _negativeChild;
    }

    const DecisionTree::Child & DecisionTree::InteriorNode::GetNegativeChild() const
    {
        return _negativeChild;
    }

    DecisionTree::Child & DecisionTree::InteriorNode::GetPositiveChild()
    {
        return _positiveChild;
    }

    const DecisionTree::Child & DecisionTree::InteriorNode::GetPositiveChild() const
    {
        return _positiveChild;
    }

    uint64_t DecisionTree::InteriorNode::NumInteriorNodesInSubtree() const
    {
        uint64_t num = 1;

        if(_negativeChild._node != nullptr)
        {
            num += _negativeChild._node->NumInteriorNodesInSubtree();
        }

        if(_positiveChild._node != nullptr)
        {
            num += _positiveChild._node->NumInteriorNodesInSubtree();
        }

        return num;
    }

    uint64_t DecisionTree::NumInteriorNodes() const
    {
        if(_root._node == nullptr)
        {
            return 0;
        }
        return _root._node->NumInteriorNodesInSubtree();
    }

    DecisionTree::Child& DecisionTree::GetRoot()
    {
        return _root;
    }

    void DecisionTree::AddToModel(layers::Model & model, layers::CoordinateList inputCoordinates) const
    {
        FlatTree flatTree;
        BuildFlatTree(flatTree, inputCoordinates, _root._node.get());

        auto thresholdsLayer = std::make_unique<layers::Coordinatewise>(std::move(flatTree.negativeThresholds), std::move(flatTree.splitRuleCoordinates), layers::Coordinatewise::OperationType::add);
        auto thresholdsLayerCoordinates = model.AddLayer(std::move(thresholdsLayer));

        auto decisionTreePathLayer = std::make_unique<layers::DecisionTreePath>(std::move(flatTree.edgeToInteriorNode), std::move(thresholdsLayerCoordinates));
        auto decisionTreePathLayerCoordinates = model.AddLayer(std::move(decisionTreePathLayer));

        auto outputValuesLayer = std::make_unique<layers::Coordinatewise>(std::move(flatTree.edgeOutputValues), std::move(decisionTreePathLayerCoordinates), layers::Coordinatewise::OperationType::multiply);
        auto outputValuesLayerCoordinates = model.AddLayer(std::move(outputValuesLayer));

        auto sumLayer = std::make_unique<layers::Sum>(std::move(outputValuesLayerCoordinates));
        model.AddLayer(std::move(sumLayer));
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

        flatTree.edgeOutputValues.push_back(interiorNodePtr->_negativeEdgeOutputValue);
        flatTree.edgeOutputValues.push_back(interiorNodePtr->_positiveEdgeOutputValue);

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
            BuildFlatTree(flatTree, inputCoordinates, interiorNodePtr->GetNegativeChild()._node.get());
            
            // set edgeToInteriorNode[positiveEdgeIndex] to its correct value
            auto positiveChildIndex = flatTree.splitRuleCoordinates.Size();
            flatTree.edgeToInteriorNode[positiveEdgeIndex] = positiveChildIndex;
            
            // recurse (DFS) to the positive side
            BuildFlatTree(flatTree, inputCoordinates, interiorNodePtr->GetPositiveChild()._node.get());
        }
    }
}
