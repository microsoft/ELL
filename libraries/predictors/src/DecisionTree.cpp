////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
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

    DecisionTree::SplitRuleResult DecisionTree::SplitRule::Evaluate(const std::vector<double>& featureVector) const
    {
        if(featureVector[_featureIndex] > _threshold)
        {
            return SplitRuleResult::positive;
        }
        else
        {
            return SplitRuleResult::negative;
        }
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

    void DecisionTree::AddToModel(layers::Model & model, const layers::CoordinateList & inputCoordinates) const
    {
        auto numInteriorNodes = NumInteriorNodes();

        auto thresholdsLayer = std::make_unique<layers::Coordinatewise>(layers::Coordinatewise::OperationType::add);
        
        auto decisionTreePathLayer = std::make_unique<layers::DecisionTreePath>();

        auto weightsLayer = std::make_unique<layers::Coordinatewise>(layers::Coordinatewise::OperationType::multiply);

        //auto coordinates = model.BuildCoordinateList(layerIndex);
        //auto sumLayer = std::make_unique<layers::Sum>(coordinates);
        //layerIndex = model.AddLayer(std::move(sumLayer));

        // TODO finish this

        model.AddLayer(std::move(thresholdsLayer));
    }
}
