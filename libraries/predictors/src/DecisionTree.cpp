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

namespace decisionTree
{
    DecisionTree::SplitRule::SplitRule(int featureIndex, double threshold) :
        _featureIndex(featureIndex), _threshold(threshold)
    {}

    int DecisionTree::SplitRule::GetFeatureIndex() const
    {
        return _featureIndex;
    }

    double DecisionTree::SplitRule::GetThreshold() const
    {
        return _threshold;
    }

    DecisionTree::ChildPair::ChildPair(int negativeChildIndex, int positiveChildIndex) :
        _negativeChildIndex(negativeChildIndex), _positiveChildIndex(positiveChildIndex)
    {}

    int DecisionTree::ChildPair::GetNegativeChildIndex() const
    {
        return _negativeChildIndex;
    }

    int DecisionTree::ChildPair::GetPositiveChildIndex() const
    {
        return _positiveChildIndex;
    }

    int DecisionTree::NumVertices() const
    {
        return (int)_parents.size();
    }

    int DecisionTree::NumInteriorVertices() const
    {
        return (int)_splitRules.size();
    }
}
