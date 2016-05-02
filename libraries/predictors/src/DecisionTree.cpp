////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     DecisionTree.cpp (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
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
    DecisionTree::SplitRule::SplitRule(int featureIndex, double threshold) :
        _featureIndex(featureIndex), _threshold(threshold)
    {}

    uint64_t DecisionTree::SplitRule::GetFeatureIndex() const
    {
        return _featureIndex;
    }

    double DecisionTree::SplitRule::GetThreshold() const
    {
        return _threshold;
    }

    DecisionTree::Child::Child(double weight) : _weight(weight)
    {}

    double DecisionTree::Child::GetWeight() const
    {
        return _weight;
    }

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

    DecisionTree::InteriorNode& DecisionTree::Child::Split(SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight)
    {
        // confirm that this is a leaf
        assert(IsLeaf());

        _node = std::make_unique<InteriorNode>(std::move(splitRule), Child(negativeLeafWeight), Child(positiveLeafWeight));
        return *_node;
    }

    DecisionTree::InteriorNode::InteriorNode(SplitRule splitRule, Child negativeChild, Child positiveChild) : _splitRule(std::move(splitRule)), _negativeChild(std::move(negativeChild)), _positiveChild(std::move(positiveChild))
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
        if(_root == nullptr)
        {
            return 0;
        }
        return _root->NumInteriorNodesInSubtree();
    }

    DecisionTree::InteriorNode & DecisionTree::SplitRoot(SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight)
    {
        // confirm that the root has never been split before
        assert(_root == nullptr);

        _root = std::make_unique<InteriorNode>(std::move(splitRule), Child(negativeLeafWeight), Child(positiveLeafWeight));
        return *_root;
    }

    void DecisionTree::AddToModel(layers::Model & model, layers::CoordinateList inputCoordinates) const
    {
        FlatTree flatTree;
        BuildFlatTree(flatTree, inputCoordinates, _root.get());

        auto thresholdsLayer = std::make_unique<layers::Coordinatewise>(std::move(flatTree.negativeThresholds), std::move(flatTree.splitRuleCoordinates), layers::Coordinatewise::OperationType::add);
        auto thresholdsLayerCoordinates = model.AddLayer(std::move(thresholdsLayer));

        auto decisionTreePathLayer = std::make_unique<layers::DecisionTreePath>(std::move(flatTree.edgeToInteriorNode), std::move(thresholdsLayerCoordinates));
        auto decisionTreePathLayerCoordinates = model.AddLayer(std::move(decisionTreePathLayer));

        auto weightsLayer = std::make_unique<layers::Coordinatewise>(std::move(flatTree.edgeWeights), std::move(decisionTreePathLayerCoordinates), layers::Coordinatewise::OperationType::multiply);
        auto weightsLayerCoordinates = model.AddLayer(std::move(weightsLayer));

        auto sumLayer = std::make_unique<layers::Sum>(std::move(weightsLayerCoordinates));
        model.AddLayer(std::move(sumLayer));
    }

    void predictors::DecisionTree::BuildFlatTree(FlatTree& flatTree, const layers::CoordinateList& inputCoordinates, InteriorNode* interiorNodePtr) const
    {
        if (interiorNodePtr == nullptr)
        {
            return;
        }

        const auto& splitRule = interiorNodePtr->GetSplitRule();
        auto splitRuleCoordinate = inputCoordinates[splitRule.GetFeatureIndex()];
        auto negativeThreshold = -(splitRule.GetThreshold());


        flatTree.splitRuleCoordinates.AddCoordinate(splitRuleCoordinate);
        flatTree.negativeThresholds.push_back(negativeThreshold);

        flatTree.edgeWeights.push_back(interiorNodePtr->GetNegativeChild()._weight);
        flatTree.edgeWeights.push_back(interiorNodePtr->GetPositiveChild()._weight);

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
