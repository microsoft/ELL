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
        struct SplitRule
        {
            uint64_t featureIndex;
            double threshold;
        };

        // early declaration of InteriorNode allows Child to point to it.
        class InteriorNode;

        class Child
        {
        public:
            /// <summary> Query if this Child is leaf. </summary>
            ///
            /// <returns> true if leaf, false if not. </returns>
            bool IsLeaf() const;

            /// <summary> Splits a leaf node </summary>
            ///
            /// <param name="splitRule"> The split rule to use. </param>
            /// <param name="negativeEdgeOutputValue"> The negative leaf output value. </param>
            /// <param name="positiveEdgeOutputValue"> The positive leaf output value. </param>
            ///
            /// <returns> Reference to the interior node that is created. </returns>
            InteriorNode& Split(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue);

        private:
            friend DecisionTree;

            std::unique_ptr<InteriorNode> _node;
        };

        /// <summary> Represents a pair of Children of a node in a binary tree. </summary>
        class InteriorNode
        {
        public:

            /// <summary> Constructs an instance of InteriorNode. </summary>
            ///
            /// <param name="splitRule"> The split rule. </param>
            /// <param name="negativeEdgeOutputValue"> The negative edge output value. </param>
            /// <param name="positiveEdgeOutputValue"> The positive edge output value. </param>
            InteriorNode(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue);

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

            friend DecisionTree;

            SplitRule _splitRule;
            double _negativeEdgeOutputValue;
            double _positiveEdgeOutputValue;
            Child _negativeChild;
            Child _positiveChild;
        };

        /// <summary> Number of interior nodes. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        uint64_t NumInteriorNodes() const;

        /// <summary> Returns a Child object that represents the tree root. </summary>
        ///
        /// <returns> The root. </returns>
        Child& GetRoot();

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> A double. </returns>
        //double Predict(const dataset::IDataVector& dataVector) const;

        /// <summary> Adds the predictor to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        void AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const;

    private:
        // represents a flattened version of the tree, where all parameters are in basic-type vectors
        struct FlatTree
        {
            layers::CoordinateList splitRuleCoordinates;
            std::vector<double> negativeThresholds;
            std::vector<uint64_t> edgeToInteriorNode;
            std::vector<double> edgeOutputValues;
        };

        void BuildFlatTree(FlatTree& flatTree, const layers::CoordinateList& inputCoordinates, InteriorNode* interiorNodePtr) const;

        Child _root;
    };
}
