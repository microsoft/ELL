////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestPredictor.h (predictors)
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
    struct ForestPredictorInteriorNodeId
    {
        size_t treeIndex;
        size_t interiorNodeIndex;
    };

    struct ForestPredictorLeafId
    {
        ForestPredictorInteriorNodeId ForestPredictorInteriorNodeId;
        size_t leafIndex;
    };

    /// <summary>
    /// Implements a forest of decision/regression trees. Split rules: Each interior node in a tree
    /// is associated with a split rule. The split rule type is set by a template argument (namely,
    /// the type can be arbitrary but a single type is used throughout the forest). A split rule is a
    /// predictor that returns an index of an outgoing edge index, or -1 to early-stop. The fan-out
    /// at each interior node can be arbitrary. Tree output: Each edge in each tree is associated
    /// with a predictor, the output of a tree is the sum of predictions made along the path from the
    /// root to a leaf, and the output of the forest is the sum over trees. The type of predictor is
    /// set by a template argument (namely, the type can be arbitrary but a single type is used
    /// throughout the forest). Note that assigning outputs to edges is equivalent to assigning them to
    /// all non-root nodes (so, outputs in leaves is a special case).
    /// </summary>
    ///
    /// <typeparam name="SplitRuleType"> Type of split rule to use in interior nodes. </typeparam>
    /// <typeparam name="EdgePredictorType"> Type of predictor to associate with each edge. </typeparam>
    template<typename SplitRuleType, typename EdgePredictorType>
    class ForestPredictor
    {
    public:
        /// <summary> Struct that defines a split rule and the predictors assigned to the outgoing edges. </summary>
        struct SplitInfo
        {
            /// <summary> The split rule. </summary>
            SplitRuleType splitRule;
            
            /// <summary> The predictors in the outgoing edges. </summary>
            std::vector<EdgePredictorType> predictors;
        };

        /// <summary> Gets the number of trees i nthe forest. </summary>
        ///
        /// <returns> The number of tress. </returns>
        size_t NumTrees() const { return _tree.size(); }

        /// <summary> Gets the total number of interior nodes in the entire forest. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        size_t NumInteriorNodes() const { return _interiorNodes.size(); }

        /// <summary> Gets the number of interior nodes in a given tree. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        size_t NumInteriorNodes(size_t treeIndex) const;

        /// <summary> Gets the number of edges in the entire forest. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges() const { return _numEdges; }

        /// <summary> Gets the number of edges in a given tree. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges(size_t treeIndex) const;

        /// <summary> Returns the output of the entire forest for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the input. </typeparam>
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The prediction. </returns>
        template<typename RandomAccessVectorType>
        double Compute(const RandomAccessVectorType& input) const;

        /// <summary> Returns the output of a given tree for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the
        /// input. </typeparam>
        /// <param name="input"> The input vector. </param>
        /// <param name="treeIndex"> Zero-based index of a tree. </param>
        ///
        /// <returns> The prediction. </returns>
        template<typename RandomAccessVectorType>
        double Compute(const RandomAccessVectorType& input, size_t treeIndex) const;

        /// <summary> Returns the edge path indicator vector of a given tree for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the
        /// input. </typeparam>
        /// <param name="input"> The input vector. </param>
        /// <param name="treeIndex"> Zero-based index of a tree. </param>
        ///
        /// <returns> The edge path indicator vector. </returns>
        template<typename RandomAccessVectorType>
        std::vector<bool> GetEdgePathIndicatorVector(const RandomAccessVectorType& input, size_t treeIndex) const;

        /// <summary> Adds a new tree (a stump) to the forest. </summary>
        ///
        /// <param name="splitInfo"> Information describing the split of the root node. </param>
        ///
        /// <returns> The ID of the newly created root node. </returns>
        ForestPredictorInteriorNodeId AddTree(const SplitInfo& splitInfo);

        /// <summary> Splits a leaf of one of the trees. </summary>
        ///
        /// <param name="splitInfo"> Information describing the split. </param>
        /// <param name="leaf"> The leaf to split. </param>
        ///
        /// <returns> The ID of the newly created interior node. </returns>
        ForestPredictorInteriorNodeId SplitLeaf(const SplitInfo& splitInfo, ForestPredictorLeafId ForestPredictorLeafId);

    protected:
        struct Edge
        {
            Edge(const EdgePredictorType& predictor);
            EdgePredictorType predictor;
            size_t targetNodeIndex;
        };

        struct InteriorNode
        {
            InteriorNode(const SplitInfo& splitInfo, size_t firstEdgeIndex);
            SplitRuleType splitRule;
            std::vector<Edge> outgoingEdges;
            size_t firstEdgeIndex;
        };

        struct Tree
        {
            size_t rootNodeIndex;
            size_t numInteriorNodes;
            size_t numEdges;
        };

        size_t AddInteriorNode(const SplitInfo& splitInfo, size_t firstEdgeIndex);

        std::vector<InteriorNode> _interiorNodes;
        std::vector<Tree> _trees;
        size_t _numEdges; 
    };

    /// <summary> A simple binary tree with single-input threshold rules and constant predictors in its edges. </summary>
    typedef ForestPredictor<SingleInputThresholdRule, ConstantPredictor> SimpleForestPredictor;
}

#include "../tcc/ForestPredictor.tcc"
