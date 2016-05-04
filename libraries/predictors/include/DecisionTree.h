////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DecisionTree.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//layers
#include "Model.h"

// stl
#include <vector>
#include <cstdint>

namespace predictors
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

            /// <summary> Gets the feature index. </summary>
            ///
            /// <returns> The feature index. </returns>
            uint64_t GetFeatureIndex() const;

            /// <summary> Gets the split rule threshold. </summary>
            ///
            /// <returns> The threshold. </returns>
            double GetThreshold() const;

        private:
            uint64_t _featureIndex;
            double _threshold;
        };

        // early declaration of InteriorNode allows Child to point to it.
        class InteriorNode;

        class Child
        {
        public:

            /// <summary> Constructs an instance of Child. </summary>
            ///
            /// <param name="weight"> The weight associated with the child. </param>
            Child(double weight);

            /// <summary> Gets the weight of the child. </summary>
            ///
            /// <returns> The weight associated with the child. </returns>
            double GetWeight() const;

            /// <summary> Query if this Child is leaf. </summary>
            ///
            /// <returns> true if leaf, false if not. </returns>
            bool IsLeaf() const;

            /// <summary> Splits a leaf node </summary>
            ///
            /// <param name="splitRule"> The split rule to use. </param>
            /// <param name="negativeLeafWeight"> The negative leaf weight. </param>
            /// <param name="positiveLeafWeight"> The positive leaf weight. </param>
            ///
            /// <returns> Reference to the interior node that is created. </returns>
            InteriorNode& Split(SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight);

        private:
            friend InteriorNode; // TODO ?
            friend DecisionTree;

            double _weight;
            std::unique_ptr<InteriorNode> _node;
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

            /// <summary> Number of interior nodes in the subtree rooted at this node. </summary>
            ///
            /// <returns> The total number of interior nodes in the subtree. </returns>
            uint64_t NumInteriorNodesInSubtree() const;

        private:
            SplitRule _splitRule;
            Child _negativeChild;
            Child _positiveChild;
        };

        /// <summary> Number of interior nodes. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        uint64_t NumInteriorNodes() const;

        /// <summary> Splits the tree root. </summary>
        ///
        /// <param name="splitRule"> The split rule to use. </param>
        /// <param name="negativeLeafWeight"> The negative leaf weight. </param>
        /// <param name="positiveLeafWeight"> The positive leaf weight. </param>
        ///
        /// <returns> Reference to the interior node that is created. </returns>
        InteriorNode& SplitRoot(SplitRule splitRule, double negativeLeafWeight, double positiveLeafWeight);

        /// <summary> Adds the predictor to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        void AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const;

    private:

        struct FlatTree
        {
            layers::CoordinateList splitRuleCoordinates;
            std::vector<double> negativeThresholds;
            std::vector<uint64_t> edgeToInteriorNode;
            std::vector<double> edgeWeights;
        };

        void BuildFlatTree(FlatTree& flatTree, const layers::CoordinateList& inputCoordinates, InteriorNode* interiorNodePtr) const;

        std::unique_ptr<InteriorNode> _root;
    };
}
