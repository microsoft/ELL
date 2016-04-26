////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DecisionTree.h (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>

namespace decisionTree
{
    /// <summary>
    /// Represents a binary decision tree with threshold split rules and output values in all
    /// vertices.
    /// </summary>
    class DecisionTree
    {

    public:
        
        /// <summary> Represents a split rule in a decision tree. </summary>
        class SplitRule
        {
        public:

            /// <summary> Constructs a SplitRule. </summary>
            ///
            /// <param name="featureIndex"> Zero-based index of the split feature. </param>
            /// <param name="threshold"> The threshold value. </param>
            SplitRule(int featureIndex, double threshold);

            /// <summary> Returns the feature index. </summary>
            ///
            /// <returns> The feature index. </returns>
            int GetFeatureIndex() const;

            /// <summary> Returns the threshold value. </summary>
            ///
            /// <returns> A threshold. </returns>
            double GetThreshold() const;

        private:
            int _featureIndex;
            double _threshold;
        };

        /// <summary> Represents a pair of Children of a node in a binary tree. </summary>
        class ChildPair 
        {
        public:

            /// <summary> Constructs the pair of Children. </summary>
            ///
            /// <param name="negativeChildIndex"> Index of the child that corresponds to instances where the split rule is negative. </param>
            /// <param name="positiveChildIndex"> Index of the child that corresponds to instances where the split rule is positive. </param>
            ChildPair(int negativeChildIndex, int positiveChildIndex);

            /// <summary> Returns index of the negative child. </summary>
            ///
            /// <returns> Index of the negative child. </returns>
            int GetNegativeChildIndex() const;

            /// <summary> Returns index of the positive child. </summary>
            ///
            /// <returns> Index of the positive child. </returns>
            int GetPositiveChildIndex() const;

        private:
            int _negativeChildIndex;
            int _positiveChildIndex;
        };

        /// <summary> Returns the number of vertices. </summary>
        ///
        /// <returns> The number vertices. </returns>
        int NumVertices() const;

        /// <summary> Returns the number of interior vertices. </summary>
        ///
        /// <returns> The number interior vertices. </returns>
        int NumInteriorVertices() const;

    private:
        std::vector<SplitRule> _splitRules;
        std::vector<ChildPair> _children;
        std::vector<int> _parents;
        std::vector<double> _outputs;
    };
}
