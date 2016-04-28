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

//layers
#include "Model.h"

// stl
#include <vector>

namespace predictors
{
    /// <summary>
    /// Represents a binary decision tree with threshold split rules and output values in all
    /// vertices.
    /// </summary>
    class DecisionTree
    {
    public:
        
        enum class SplitRuleResult {negative, positive};

        /// <summary> Represents a split rule in a decision tree. </summary>
        class SplitRule
        {
        public:

            /// <summary> Constructs a SplitRule. </summary>
            ///
            /// <param name="featureIndex"> Zero-based index of the split feature. </param>
            /// <param name="threshold"> The threshold value. </param>
            SplitRule(int featureIndex, double threshold);

            /// <summary> Evaluates the split rule on a given feature vector. </summary>
            ///
            /// <param name="featureVector"> The feature vector. </param>
            ///
            /// <returns> A SplitRuleResult. </returns>
            SplitRuleResult Evaluate(const std::vector<double>& featureVector) const;

        private:
            int _featureIndex;
            double _threshold;
        };

        class Child
        {
        public:

            /// <summary> Constructs an instance of Child. </summary>
            ///
            /// <param name="weight"> The weight associated with the child. </param>
            /// <param name="index"> The index of the child, if it is an interior node; zero otherwise. </param>
            Child(double weight, uint64_t index=0);

            /// <summary> Gets the weight of the child. </summary>
            ///
            /// <returns> The weight associated with the child. </returns>
            double GetWeight() const;

            /// <summary> Gets the index of the child, if it is an interior node; zero otherwise. </summary>
            ///
            /// <returns> The index, or zero for leaves
            ///           . </returns>
            uint64_t GetIndex() const;

        private:
            double _weight;
            uint64_t _index;
            friend DecisionTree;
        };

        /// <summary> Represents a pair of Children of a node in a binary tree. </summary>
        class InteriorNode
        {
        public:
            
            /// <summary> Constructs an instance of InteriorNode. </summary>
            ///
            /// <param name="splitRule"> The split rule. </param>
            /// <param name="negativeChild"> The negative child. </param>
            /// <param name="positiveChild"> The positive child. </param>
            InteriorNode(SplitRule splitRule, Child negativeChild, Child positiveChild);

            /// <summary> Gets the split rule. </summary>
            ///
            /// <returns> The split rule. </returns>
            const SplitRule& GetSplitRule() const;

            /// <summary> Gets the negative child of this interior node. </summary>
            ///
            /// <returns> The negative child. </returns>
            Child& GetNegativeChild();

            /// <summary> Gets the negative child of this interior node. </summary>
            ///
            /// <returns> The negative child. </returns>
            const Child& GetNegativeChild() const;

            /// <summary> Gets the positive child of this interior node. </summary>
            ///
            /// <returns> The positive child. </returns>
            Child& GetPositiveChild();

            /// <summary> Gets the positive child of this interior node. </summary>
            ///
            /// <returns> The positive child. </returns>
            const Child& GetPositiveChild() const;

        private:
            SplitRule _splitRule;
            Child _negativeChild;
            Child _positiveChild;
        };

        /// <summary> Returns the number of vertices. </summary>
        ///
        /// <returns> The number vertices. </returns>
        uint64_t NumNodes() const;

        /// <summary> Returns the number of interior vertices. </summary>
        ///
        /// <returns> The number interior vertices. </returns>
        uint64_t NumInteriorNodes() const;

        /// <summary> Splits a leaf, turning it into an interior node. </summary>
        ///
        /// <param name="child"> [in,out] The leaf being split. Must be a leaf in this tree. </param>
        /// <param name="splitRule"> The split rule to use. </param>
        /// <param name="negativeLeafWeight"> The negative leaf weight. </param>
        /// <param name="positiveLeafWeight"> The positive leaf weight. </param>
        ///
        /// <returns> Reference to the interior node that is created. </returns>
        InteriorNode& SplitLeaf(Child& child, SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight);

        /// <summary> Adds the predictor to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        void AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;

    private:
        std::vector<InteriorNode> _interiorNodes;
    };
}
