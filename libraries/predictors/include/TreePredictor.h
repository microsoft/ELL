////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TreePredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SingleInputThresholdRule.h"
#include "ConstantPredictor.h"

// dataset
#include "DenseDataVector.h"

// stl
#include <vector>
#include <algorithm>
#include <iterator>

namespace predictors
{
    /// <summary>
    /// Implements a tree predictor. Split rules: Each interior node is associated with a split rule.
    /// The split rule type is set by a template argument (namely, the type can be arbitrary but a
    /// single type is used throughout the tree). A split rule returns an outgoing edge index, or
    /// -1 to early-stop. The fan-out at each interior node can be arbitrary. Tree output: Each edge
    /// in the tree is associated with a predictor and the output of the tree is the sum of
    /// predictions made along the path from the root to a leaf. The type of predictor is set by a
    /// template argument (namely, the type can be arbitrary but a single type is used throughout the
    /// tree). Note that assigning outputs to edges is equivalent to assigning them to all non-root
    /// nodes, and therefore associating outputs with leaves is a special case. If a split rule along
    /// the path returns -1, the output is the sum of predictions made so far.
    /// </summary>
    ///
    /// <typeparam name="SplitRuleType"> Type of split rule to use. </typeparam>
    /// <typeparam name="EdgePredictorType"> Type of predictor to associate with each edge. </typeparam>
    template<typename SplitRuleType, typename EdgePredictorType>
    class TreePredictor
    {
    public:
        /// <summary> Struct that represents a leaf in the tree. </summary>
        struct Leaf
        {
            size_t interiorNodeIndex;
            size_t leafIndex;
        };

        struct SplitInfo
        {
            SplitRuleType splitRule;
            std::vector<EdgePredictorType> predictors;
        };


        /// <summary> Information need to split a leaf of the tree. </summary>
        struct SplitCandidate
        {
            /// <summary> The leaf to split. </summary>
            Leaf leaf;
            SplitInfo splitInfo;
        };

        /// <summary> Gets the number of interior nodes. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        size_t NumInteriorNodes() const { return _interiorNodes.size(); }

        /// <summary> Gets the number of edges. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges() const { return _numEdges; }

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
        size_t Split(const SplitInfo& splitInfo);

    protected:

        struct EdgeData // TODO remove the Data prefix everwhere?
        {
            EdgeData(const EdgePredictorType& predictor) : predictor(predictor), targetNodeIndex(0) {} // TODO
            EdgePredictorType predictor;
            size_t targetNodeIndex;
        };

        struct InteriorNodeData
        {
            InteriorNodeData(const SplitInfo& splitInfo) : splitRule(splitInfo.splitRule) 
            {
                std::copy(splitInfo.predictors.begin(), splitInfo.predictors.end(), std::back_inserter(outgoingEdges));
            }
            SplitRuleType splitRule;
            std::vector<EdgeData> outgoingEdges;
        };

        std::vector<InteriorNodeData> _interiorNodes;
        size_t _numEdges;
    };

    /// <summary> A simple binary tree with single-input threshold rules. </summary>
    typedef TreePredictor<SingleInputThresholdRule, ConstantPredictor> SimpleTreePredictor;
}

#include "../tcc/TreePredictor.tcc"
