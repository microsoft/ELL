////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DecisionTreePredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "IDataVector.h"
#include "DenseDataVector.h"

//layers
#include "Model.h"

// stl
#include <vector>
#include <cstdint>

/// <summary> predictors namespace </summary>
namespace predictors
{
    /// <summary>
    /// Represents a binary decision tree with threshold split rules and output values in all
    /// vertices.
    /// </summary>
    class DecisionTreePredictor
    {
    public:
        
        /// <summary> Represents a split rule in a decision tree. </summary>
        struct SplitRule
        {
            uint64_t featureIndex;
            double threshold;
        };

        // early declaration of InteriorNode allows Node to point to it.
        class InteriorNode;

        class Node
        {
        public:

            /// <summary> Constructs an instance of Node. </summary>
            ///
            /// <param name="outputValue"> The output value. </param>
            Node(double outputValue);

            /// <summary> Gets the node's output value. </summary>
            ///
            /// <returns> The output value. </returns>
            double getOutputValue() const { return _outputValue; }
            
            /// <summary> Query if this Node is leaf. </summary>
            ///
            /// <returns> true if leaf, false if not. </returns>
            bool IsLeaf() const { return _interiorNode == nullptr; }

            /// <summary> Returns the output of the subtree rooted at this node for a given example. </summary>
            ///
            /// <param name="example"> The data vector. </param>
            ///
            /// <returns> The prediction. </returns>
            double Predict(const dataset::DoubleDataVector& dataVector) const;

            /// <summary> Splits a leaf node </summary>
            ///
            /// <param name="splitRule"> The split rule to use. </param>
            /// <param name="negativeEdgeOutputValue"> The negative leaf output value. </param>
            /// <param name="positiveEdgeOutputValue"> The positive leaf output value. </param>
            ///
            /// <returns> Reference to the interior node that is created. </returns>
            InteriorNode& Split(SplitRule splitRule, double negativeEdgeOutputValue, double positiveEdgeOutputValue);

        private:
            friend DecisionTreePredictor;

            double _outputValue;
            std::unique_ptr<InteriorNode> _interiorNode;
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
            InteriorNode(SplitRule splitRule, Node negativeChild, Node positiveChild);

            /// <summary> Gets the split rule. </summary>
            ///
            /// <returns> The split rule. </returns>
            const SplitRule& GetSplitRule() const { return _splitRule; }

            /// <summary> Gets the negative child of this interior node. </summary>
            ///
            /// <returns> The negative child. </returns>
            Node& GetNegativeChild() { return _negativeChild; }

            /// <summary> Gets the negative child of this interior node. </summary>
            ///
            /// <returns> The negative child. </returns>
            const Node& GetNegativeChild() const { return _negativeChild; }

            /// <summary> Gets the positive child of this interior node. </summary>
            ///
            /// <returns> The positive child. </returns>
            Node& GetPositiveChild() { return _positiveChild; }

            /// <summary> Gets the positive child of this interior node. </summary>
            ///
            /// <returns> The positive child. </returns>
            const Node& GetPositiveChild() const { return _positiveChild; }

            /// <summary> Number of interior nodes in the subtree rooted at this node. </summary>
            ///
            /// <returns> The total number of interior nodes in the subtree. </returns>
            uint64_t NumInteriorNodesInSubtree() const;

            /// <summary> Returns the output of the subtree rooted at this node for a given example. </summary>
            ///
            /// <param name="example"> The data vector. </param>
            ///
            /// <returns> The prediction. </returns>
            double Predict(const dataset::DoubleDataVector& dataVector) const;

        private:
            friend DecisionTreePredictor;

            SplitRule _splitRule;
            Node _negativeChild;
            Node _positiveChild;
        };

        /// <summary> Constructs an instance of DecisionTreePredictor. </summary>
        ///
        /// <param name="rootOutputValue"> The root output value. </param>
        DecisionTreePredictor(double rootOutputValue);

        /// <summary> Number of nodes in the tree. </summary>
        ///
        /// <returns> The number of nodes. </returns>
        uint64_t NumNodes() const;

        /// <summary> Number of interior nodes in the tree. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        uint64_t NumInteriorNodes() const;

        /// <summary> Returns a Node object that represents the tree root. </summary>
        ///
        /// <returns> The root. </returns>
        Node& GetRoot() { return _root; }

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> The prediction.</returns>
        double Predict(const dataset::IDataVector& dataVector) const;

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
            std::vector<double> nonRootOutputValues;
        };

        void BuildFlatTree(FlatTree& flatTree, const layers::CoordinateList& inputCoordinates, InteriorNode* interiorNodePtr) const;

        // member variables
        Node _root;
    };
}
