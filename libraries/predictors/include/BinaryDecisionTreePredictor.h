////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryDecisionTreePredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "DenseDataVector.h"

// stl
#include <vector>
#include <array>

namespace predictors
{
    /// <summary> Implements a proper binary decision tree ('proper binary' means that each interior
    /// node has exactly two children). Each edge is assigned a weight and the output of the tree is
    /// the sum along the path from the tree root to a leaf. Note that assigning weights to edges is
    /// equivalent to assigning weights to all nodes other than the root. </summary>
    class BinaryDecisionTreePredictor
    {
    public:

        /// <summary> Struct that represents a leaf in the tree. </summary>
        struct Leaf
        {
            size_t interiorNodeIndex;
            bool leafIndex;
        };

        /// <summary> A split rule that compares a single feature to a threshold. </summary>
        class Rule
        {
        public:

            /// <summary> Constructs a rule. </summary>
            ///
            /// <param name="inputIndex"> Zero-based index of the input coordinate. </param>
            /// <param name="threshold"> The threshold. </param>
            Rule(size_t inputIndex, double threshold);

            /// <summary> Evaluates the split rule. </summary>
            ///
            /// <param name="dataVector"> The data vector. </param>
            ///
            /// <returns> The result of the split rule. </returns>
            size_t operator()(const dataset::DoubleDataVector& dataVector) const;

        private:
            size_t _inputIndex;
            double _threshold;
        };

        /// <summary> Information need to split a leaf of the tree. </summary>
        struct SplitInfo
        {
            /// <summary> The leaf to split. </summary>
            Leaf leaf;

            /// <summary> The rule in the new interior node. </summary>
            Rule rule;

            /// <summary> The weights of the two new edges. </summary>
            std::array<double, 2> edgeWeights;
        };

        /// <summary> Gets the number of interior nodes. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        size_t NumInteriorNodes() const { _interiorNodes.size(); }

        /// <summary> Gets the number of edges. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges() const { return 2 * _interiorNodes.size(); }

        /// <summary> Returns the output of the tree for a given input. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The prediction. </returns>
        double Predict(const dataset::DoubleDataVector& input) const;
        
        /// <summary> Returns the edge path indicator vector for a given input. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The edge path indicator vector. </returns>
        std::vector<bool> GetEdgePathIndicator(const dataset::DoubleDataVector& input) const;

        /// <summary> Performs a split in the tree. </summary>
        ///
        /// <param name="splitInfo"> Information describing the split. </param>
        ///
        /// <returns> Index of the newly created interior node. </returns>
        size_t Split(const SplitInfo& splitInfo);

    protected:
        struct EdgeData
        {
            EdgeData(double weight);
            double weight;
            size_t targetNodeIndex;
        };

        struct InteriorNodeData
        {
            Rule rule;
            std::array<EdgeData, 2> edgeData;
        };

        std::vector<InteriorNodeData> _interiorNodes;
    };
}