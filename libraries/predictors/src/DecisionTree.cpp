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

    DecisionTree::InteriorNode & DecisionTree::SplitRoot(SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight)
    {
        // confirm that the root has never been split before
        assert(_root == nullptr);

        _root = std::make_unique<InteriorNode>(std::move(splitRule), Child(negativeLeafWeight), Child(positiveLeafWeight));
        return *_root;
    }

    void DecisionTree::AddToModel(layers::Model & model, const layers::CoordinateList & inputCoordinates) const
    {
        // TODO
        // add theshold layer

        // add path layer

        // add weight layer

        // add sum layer
    }
}
