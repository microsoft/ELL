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

    DecisionTree::Child::Child(double weight, uint64_t index) : _weight(weight), _index()
    {}

    double DecisionTree::Child::GetWeight() const
    {
        return _weight;
    }

    uint64_t DecisionTree::Child::GetIndex() const
    {
        return _index;
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

    uint64_t DecisionTree::NumNodes() const
    {
        return _interiorNodes.size() * 2 + 1;
    }

    uint64_t DecisionTree::NumInteriorNodes() const
    {
        return _interiorNodes.size();
    }

    DecisionTree::InteriorNode& DecisionTree::SplitLeaf(Child& child, SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight)
    {
        // only split leaves
        assert(child._index == 0);

        child._index = _interiorNodes.size();
        _interiorNodes.emplace_back(std::move(splitRule), Child(negativeLeafWeight), Child(positiveLeafWeight));
        return _interiorNodes.back();
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
