////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstantPredictor.h"
#include "IPredictor.h"
#include "SingleElementThresholdPredictor.h"

// data
#include "DenseDataVector.h"

// utilities
#include "IArchivable.h"

// stl
#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

namespace ell
{
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
    template <typename SplitRuleType, typename EdgePredictorType>
    class ForestPredictor : public IPredictor<double>, public utilities::IArchivable
    {
    public:
        /// <summary> A struct that identifies a splittable node in the forest. The splittable node can be
        /// the root of a new tree, or a node in an existing tree. This stuct can only be created by
        /// calling GetNewRootId() or GetChildId(). </summary>
        class SplittableNodeId
        {
        public:
            /// <summary> Prints the node Id. </summary>
            ///
            /// <param name="os"> The output stream. </param>
            void Print(std::ostream& os) const;

        private:
            friend ForestPredictor<SplitRuleType, EdgePredictorType>;
            SplittableNodeId()
                : _isRoot(true) {}
            SplittableNodeId(size_t parentNodeIndex, size_t childPosition);

            bool _isRoot;
            size_t _parentNodeIndex;
            size_t _childPosition;
        };

        /// <summary> Struct that defines a split rule and the predictors assigned to the outgoing edges. </summary>
        class SplitAction
        {
        public:
            /// <summary> Constructs an instance of SplitAction. </summary>
            ///
            /// <param name="nodeId"> Identifier for the node to split. </param>
            /// <param name="_splitRule"> The split rule. </param>
            /// <param name="predictors"> The edge predictors to use. </param>
            SplitAction(SplittableNodeId nodeId, SplitRuleType _splitRule, std::vector<EdgePredictorType> edgePredictors);

            /// <summary> Gets the split rule. </summary>
            ///
            /// <returns> The split rule. </returns>
            const SplitRuleType& GetSplitRule() const { return _splitRule; }

            /// <summary> Prints the split action to an output stream on a separate line. </summary>
            ///
            /// <param name="os"> The output stream. </param>
            /// <param name="tabs"> The number of tabs. </param>
            void PrintLine(std::ostream& os, size_t tabs = 0) const;

        private:
            friend ForestPredictor<SplitRuleType, EdgePredictorType>;
            SplittableNodeId _nodeId;
            SplitRuleType _splitRule;
            std::vector<EdgePredictorType> _edgePredictors;
        };

        class Edge : public utilities::IArchivable
        {
        public:
            Edge() = default;

            /// <summary> Constructs an instance of Edge. </summary>
            ///
            /// <param name="predictor"> The predictor. </param>
            Edge(const EdgePredictorType& predictor);

            /// <summary> Gets the target node index. </summary>
            ///
            /// <returns> The target node index. </returns>
            size_t GetTargetNodeIndex() const { return _targetNodeIndex; }

            /// <summary> Gets the predictor associated with this edge. </summary>
            ///
            /// <returns> The edge predictor. </returns>
            const EdgePredictorType& GetPredictor() const { return _predictor; }

            /// <summary> Determines if the target of this edge is an interior node. </summary>
            ///
            /// <returns> true if the target is an interior node. </returns>
            bool IsTargetInterior() const;

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return "Edge"; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Prints a human readable description of the edge, indented by a given number of tabs - used for debugging. </summary>
            ///
            /// <param name="os"> [in,out] Stream to write data to. </param>
            /// <param name="tabs"> The tabs. </param>
            void PrintLine(std::ostream& os, size_t tabs = 0) const;

        protected:
            virtual void WriteToArchive(utilities::Archiver& archiver) const override;
            virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            friend ForestPredictor<SplitRuleType, EdgePredictorType>;
            void SetTargetNodeIndex(size_t targetNodeIndex);
            EdgePredictorType _predictor;
            size_t _targetNodeIndex = 0;
        };

        /// <summary> Represents an interior node of one of the trees in the forest. </summary>
        class InteriorNode : public utilities::IArchivable
        {
        public:
            InteriorNode() = default;

            /// <summary> Gets the split rule. </summary>
            ///
            /// <returns> The split rule. </returns>
            const SplitRuleType& GetSplitRule() const { return _splitRule; }

            /// <summary> Gets the vector of outgoing edges. </summary>
            ///
            /// <returns> The outgoing edges. </returns>
            const std::vector<Edge>& GetOutgoingEdges() const { return _outgoingEdges; }

            /// <summary> Gets the index of the first outgoung edge. </summary>
            ///
            /// <returns> The first edge index. </returns>
            size_t GetFirstEdgeIndex() const { return _firstEdgeIndex; }

            /// <summary> Prints a human readable description of the interior node, indented by a given number of tabs - used for debugging. </summary>
            ///
            /// <param name="os"> [in,out] The output stream. </param>
            /// <param name="tabs"> The number of tabs. </param>
            void PrintLine(std::ostream& os, size_t tabs = 0) const;

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return "InteriorNode"; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        protected:
            virtual void WriteToArchive(utilities::Archiver& archiver) const override;
            virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            friend ForestPredictor<SplitRuleType, EdgePredictorType>;
            InteriorNode(const SplitAction& splitAction, size_t _firstEdgeIndex);
            SplitRuleType _splitRule;
            std::vector<Edge> _outgoingEdges;
            size_t _firstEdgeIndex = 0;
        };

        /// <summary> Type of the data vector expected by this predictor type. </summary>
        using DataVectorType = data::FloatDataVector;

        /// <summary> Query if this forest has no trees and a zero bias. </summary>
        ///
        /// <returns> true if the forest is trivial. </returns>
        bool IsTrivial() const;

        /// <summary> Gets the number of trees in the forest. </summary>
        ///
        /// <returns> The number of tress. </returns>
        size_t NumTrees() const { return _rootIndices.size(); }

        /// <summary> Gets the index of the root node of a given tree. </summary>
        ///
        /// <param name="treeIndex"> The tree index. </param>
        ///
        /// <returns> The root index. </returns>
        size_t GetRootIndex(size_t treeIndex) const { return _rootIndices[treeIndex]; }

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

        /// <summary> Returns the output of the forest (including all trees and the bias term) for a given input. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The prediction. </returns>
        double Predict(const DataVectorType& input) const;

        /// <summary> Returns the output of a given subtree for a given input. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        /// <param name="treeIndex"> The index of the subtree root. </param>
        ///
        /// <returns> The prediction. </returns>
        double Predict(const DataVectorType& input, size_t interiorNodeIndex) const;

        /// <summary> Generates the edge path indicator vector of the entire forest. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        ///
        /// <returns> The edge indicator vector. </returns>
        std::vector<bool> GetEdgeIndicatorVector(const DataVectorType& input) const;

        /// <summary> Generates the edge path indicator vector of a given subtree for a given input. The
        /// dimension of this vector is NumEdges() (regardless of the subtree chosen). </summary>
        ///
        /// <param name="input"> The input vector. </param>
        /// <param name="interiorNodeIndex"> Zero-based index of the interior node. </param>
        ///
        /// <returns> The edge indicator vector. </returns>
        std::vector<bool> GetEdgeIndicatorVector(const DataVectorType& input, size_t interiorNodeIndex) const;

        /// <summary> Gets a SplittableNodeId that represents the root of a new tree. </summary>
        ///
        /// <returns> A root node identifier. </returns>
        SplittableNodeId GetNewRootId() const { return {}; }

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

        /// <summary> Gets the bias value. </summary>
        ///
        /// <returns> The bias. </returns>
        double GetBias() const { return _bias; }

        /// <summary> Adds a constant value to the bias term. </summary>
        ///
        /// <param name="value"> The value. </param>
        void AddToBias(double value);

        /// <summary> Gets a vector of interior nodes in a topological order. </summary>
        ///
        /// <returns> The vector of interior nodes. </returns>
        const std::vector<InteriorNode>& GetInteriorNodes() const { return _interiorNodes; }

        /// <summary> Gets a vector of tree root indices. </summary>
        ///
        /// <returns> The vector of tree root indices. </returns>
        const std::vector<size_t>& GetRootIndices() const { return _rootIndices; }

        /// <summary> Prints a representation of the forest to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        /// <param name="tabs"> The number of tabs. </param>
        void PrintLine(std::ostream& os, size_t tabs = 0) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<SplitRuleType, EdgePredictorType>("ForestPredictor"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        //
        // protected member functions
        //
        void SetEdgeIndicatorVector(const DataVectorType& input, std::vector<bool>& edgeIndicator, size_t interiorNodeIndex) const;

        size_t AddInteriorNode(const SplitAction& splitAction);

        void VisitEdgePathToLeaf(const DataVectorType& input, size_t interiorNodeIndex, std::function<void(const InteriorNode&, size_t edgePosition)> operation) const;

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        //
        //  member variables
        //
        std::vector<InteriorNode> _interiorNodes;
        std::vector<size_t> _rootIndices;
        double _bias = 0.0;
        size_t _numEdges = 0;
    };

    /// <summary> A simple binary tree with single-input threshold rules and constant predictors in its edges. </summary>
    typedef ForestPredictor<SingleElementThresholdPredictor, ConstantPredictor> SimpleForestPredictor;
}
}

#include "../tcc/ForestPredictor.tcc"
