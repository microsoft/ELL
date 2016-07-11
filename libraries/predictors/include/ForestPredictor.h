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
    /// <summary> Implements a forest of decision/regression trees. Split rules: Each interior node in
    /// each tree is associated with a split rule. The split rule type is set by a template argument
    /// (namely, the type can be arbitrary but a single type is used throughout the forest). A split
    /// rule is a predictor that returns an index of an outgoing edge, or -1 to early-stop the path
    /// in a tree. The fan-out at each interior node can be arbitrary. Tree output: Each edge in each
    /// tree is associated with a predictor, the output of a tree is the sum of predictions made
    /// along the path from the root to a leaf, and the output of the forest is the sum over trees.
    /// The type of predictor is set by a template argument (namely, the type can be arbitrary but a
    /// single type is used throughout the forest). Note that assigning outputs to edges is
    /// equivalent to assigning them to all non-root nodes (so, outputs in leaves is a special case). </summary>
    ///
    /// <typeparam name="SplitRuleType"> Type of split rule to use in interior nodes. </typeparam>
    /// <typeparam name="EdgePredictorType"> Type of predictor to associate with each edge. </typeparam>
    template<typename SplitRuleType, typename EdgePredictorType>
    class ForestPredictor
    {
    public:
        /// <summary> A struct that identifies a splittable node in the forest. The splittable node can be
        /// the root of a new tree, or a node in an existing tree. </summary>
        struct SplittableNodeId // TODO: give it a private ctor and make the forest a friend
        {
            bool isRoot;
            size_t parentNodeIndex;
            size_t childPosition;
        };

        /// <summary> Struct that defines a split rule and the predictors assigned to the outgoing edges. </summary>
        struct SplitAction
        {

            /// <summary> Identifier of the node to split. </summary>
            SplittableNodeId nodeId;

            /// <summary> The split rule. </summary>
            SplitRuleType splitRule;
            
            /// <summary> The predictors in the outgoing edges. </summary>
            std::vector<EdgePredictorType> predictors;


        };

        /// <summary> Gets the number of trees in the forest. </summary>
        ///
        /// <returns> The number of tress. </returns>
        size_t NumTrees() const { return _trees.size(); }

        /// <summary> Gets the index of the root node of a given tree. </summary>
        ///
        /// <param name="treeIndex"> The tree index. </param>
        ///
        /// <returns> The root index. </returns>
        size_t GetRootIndex(size_t treeIndex) const { return _trees[treeIndex].rootIndex; }

        /// <summary> Gets the total number of interior nodes in the entire forest. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        size_t NumInteriorNodes() const { return _interiorNodes.size(); }

        /// <summary> Gets the number of interior nodes in the subtree under a given interior node. </summary>
        ///
        /// <returns> The number of interior nodes under a given interior node. </returns>
        size_t NumInteriorNodes(size_t interiorNodeIndex) const;

        /// <summary> Gets the number of edges in the entire forest. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges() const { return _numEdges; }

        /// <summary> Gets the number of edges in the subtree under a given interior node. </summary>
        ///
        /// <returns> The number of edges. </returns>
        size_t NumEdges(size_t interiorNodeIndex) const;

        /// <summary> Returns the output of the forest for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the input. </typeparam>
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The prediction. </returns>
        template<typename RandomAccessVectorType>
        double Compute(const RandomAccessVectorType& input) const;

        /// <summary> Returns the output of a given subtree for a given input. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the
        /// input. </typeparam>
        /// <param name="input"> The input vector. </param>
        /// <param name="treeIndex"> The index of the subtree root. </param>
        ///
        /// <returns> The prediction. </returns>
        template<typename RandomAccessVectorType>
        double Compute(const RandomAccessVectorType& input, size_t interiorNodeIndex) const;

        /// <summary> Generates the edge path indicator vector of the entire forest. </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the
        ///  input. </typeparam>
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The edge indicator vector. </returns>
        template<typename RandomAccessVectorType>
        std::vector<bool> GetEdgeIndicatorVector(const RandomAccessVectorType& input) const;

        /// <summary> Generates the edge path indicator vector of a given subtree for a given input. The
        /// dimension of this vector is NumEdges() (regardless of the subtree chosen). </summary>
        ///
        /// <typeparam name="RandomAccessVectorType"> The random access vector type used to represent the
        ///  input. </typeparam>
        /// <param name="input"> The input vector. </param>
        /// <param name="interiorNodeIndex"> Zero-based index of the interior node. </param>
        ///
        /// <returns> The edge indicator vector. </returns>
        template<typename RandomAccessVectorType>
        std::vector<bool> GetEdgeIndicatorVector(const RandomAccessVectorType& input, size_t interiorNodeIndex) const;

        /// <summary> Gets a SplittableNodeId that represents the root of a new tree. </summary>
        ///
        /// <returns> A root node identifier. </returns>
        SplittableNodeId GetNewRootId() const { return {true, 0, 0}; }

        /// <summary> Gets the number of children of a given interior node. </summary>
        ///
        /// <param name="interiorNodeIndex"> Index of the interior node. </param>
        ///
        /// <returns> The number of children. </returns>
        size_t NumChildren(size_t interiorNodeIndex) const;

        /// <summary> Gets a SplittableNodeId that represents the child of an interior node. </summary>
        ///
        /// <param name="parentNodeIndex"> Index of the parent node. </param>
        /// <param name="childPosition"> The position of the child. </param>
        ///
        /// <returns> The child identifier. </returns>
        SplittableNodeId GetChildId(size_t parentNodeIndex, size_t childPosition) const;

        /// <summary> Splits a leaf of one of the existing trees or the root of a new tree. </summary>
        ///
        /// <param name="splitAction"> Information describing the split. </param>
        ///
        /// <returns> The index of the newly created interior node. </returns>
        size_t Split(const SplitAction& splitAction);

    protected:
        struct Edge
        {
            Edge(const EdgePredictorType& predictor);
            EdgePredictorType predictor;
            size_t targetNodeIndex;
        };

        struct InteriorNode
        {
            InteriorNode(const SplitAction& splitAction, size_t firstEdgeIndex);
            SplitRuleType splitRule;
            std::vector<Edge> outgoingEdges;
            size_t firstEdgeIndex;
        };

        struct Tree
        {
            size_t rootIndex;
        };

        template<typename RandomAccessVectorType>
        void SetEdgeIndicatorVector(const RandomAccessVectorType& input, std::vector<bool>& edgeIndicator, size_t interiorNodeIndex) const;

        size_t AddInteriorNode(const SplitAction& splitAction);

        std::vector<InteriorNode> _interiorNodes;
        std::vector<Tree> _trees;
        size_t _numEdges = 0; 
    };

    /// <summary> A simple binary tree with single-input threshold rules and constant predictors in its edges. </summary>
    typedef ForestPredictor<SingleInputThresholdRule, ConstantPredictor> SimpleForestPredictor;
}

#include "../tcc/ForestPredictor.tcc"
