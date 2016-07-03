////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TreePredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SingleInputThresholdRule.h"

// dataset
#include "DenseDataVector.h"

// stl
#include <vector>
#include <array>

namespace predictors
{
    /// <summary>
    /// Implements a proper binary decision tree ('proper binary' means that all interior nodes have
    /// the same fan-out). Each edge is assigned a weight and the output of the tree is the sum
    /// along the path from the tree root to a leaf. Note that assigning weights to edges is
    /// equivalent to assigning weights to all nodes other than the root.
    /// </summary>
    ///
    /// <typeparam name="SplitRuleType"> Type of split rule to use (also determines the fan-out). </typeparam>
    template<typename SplitRuleType>
    class TreePredictor
    {
    public:
        /// <summary> The fan out of interior nodes. </summary>
        static constexpr size_t FanOut = SplitRuleType::NumOutputs;

        /// <summary> Struct that represents a leaf in the tree. </summary>
        struct Leaf
        {
            size_t interiorNodeIndex;
            size_t leafIndex;
        };

        /// <summary> Information need to split a leaf of the tree. </summary>
        struct SplitInfo
        {
            /// <summary> The leaf to split. </summary>
            Leaf leaf;

            /// <summary> The rule in the new interior node. </summary>
            SplitRuleType splitRule;

            /// <summary> The weights of the outgoing edges. </summary>
            std::array<double, FanOut> edgeWeights;
        };

        /// <summary> Gets the number of interior nodes. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        size_t NumInteriorNodes() const { _interiorNodes.size(); }

        /// <summary> Gets the number of edges. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges() const;

        /// <summary> Returns the output of the tree for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the input. </typeparam>
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The prediction. </returns>
        template<typename RandomAccessVectorType>
        double Compute(const RandomAccessVectorType& input) const;

        /// <summary> Returns the edge path indicator vector for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the input. </typeparam>
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The edge path indicator vector. </returns>
        template<typename RandomAccessVectorType>
        std::vector<bool> GetEdgePathIndicatorVector(const RandomAccessVectorType& input) const;

        /// <summary> Performs a split in the tree. </summary>
        ///
        /// <param name="splitInfo"> Information describing the split. </param>
        ///
        /// <returns> Index of the newly created interior node. </returns>
        //size_t Split(const SplitInfo& splitInfo);

    protected:
        struct EdgeData
        {
            EdgeData(double weight);
            double weight;
            size_t targetNodeIndex;
        };

        struct InteriorNodeData
        {
            SplitRuleType splitRule;
            std::array<EdgeData, FanOut> outgoingEdges;
        };

        std::vector<InteriorNodeData> _interiorNodes;
    };

    /// <summary> A simple binary tree with single-input threshold rules. </summary>
    typedef TreePredictor<SingleInputThresholdRule> SimpleTreePredictor;
}