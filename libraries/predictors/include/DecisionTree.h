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

// stl
#include <vector>

namespace decisionTree
{
    /// <summary>
    /// Represents a growable binary decision tree with interval splitting rules and outputs in all
    /// vertices.
    /// </summary>
    class DecisionTree
    {

    public:
        
        /// <summary> Represents a SplitRule in a decision tree. </summary>
        class SplitRule
        {
        public:

            /// <summary> Constructs a SplitRule. </summary>
            ///
            /// <param name="index"> Zero-based index of the. </param>
            /// <param name="value"> The value. </param>
            SplitRule(int index, double value);

            /// <summary> Returns the index. </summary>
            ///
            /// <returns> The index. </returns>
            int GetIndex() const;

            /// <summary> Returns the value. </summary>
            ///
            /// <returns> A double. </returns>
            double Get() const;

        private:
            int _index;
            double _value;
        };

        /// <summary> Represents a pair of Children of a node in a binary tree. </summary>
        class ChildPair 
        {
        public:

            /// <summary> Constructs the pair of Children. </summary>
            ///
            /// <param name="child0"> Child 0. </param>
            /// <param name="child1"> Child 1. </param>
            ChildPair(int child0, int child1);

            /// <summary> Returns Child 0. </summary>
            ///
            /// <returns> Child 0. </returns>
            int GetChild0() const;

            /// <summary> Returns Child 1. </summary>
            ///
            /// <returns> Child 1. </returns>
            int GetChild1() const;

            /// <summary> Sets child 0. </summary>
            ///
            /// <param name="index"> Child 0. </param>
            void SetChild0(int index);

            /// <summary> Sets child 1. </summary>
            ///
            /// <param name="index"> Child 1. </param>
            void SetChild1(int index);

        private:
            int _child0;
            int _child1;
        };

        /// <summary> Constructs an empty mutable decision tree with one leaf, the root. </summary>
        ///
        /// <param name="root_output"> The root output value. </param>
        DecisionTree(double root_output = 0.0);

        /// <summary> An iterator used to expose the tree split rules. </summary>
        using SplitRuleIterator = std::vector<SplitRule>::const_iterator;

        /// <summary>
        /// Returns An iterator over the split rules that points to the first SplitRule.
        /// </summary>
        ///
        /// <returns> A SplitRuleIterator. </returns>
        SplitRuleIterator SplitRuleBegin() const;

        /// <summary>
        /// Returns An iterator over the split rules that points beyond the last SplitRule.
        /// </summary>
        ///
        /// <returns> A SplitRuleIterator. </returns>
        SplitRuleIterator SplitRuleEnd() const;

        /// <summary>
        /// An iterator used to expose the parent identities of vertices in the the tree.
        /// </summary>
        using ParentIterator = std::vector<int>::const_iterator;

        /// <summary>
        /// Returns An iterator over parent indentities that points to the first vertex.
        /// </summary>
        ///
        /// <returns> A ParentIterator. </returns>
        ParentIterator ParentBegin() const;

        /// <summary>
        /// Returns An iterator over parent identities that points beyond the last vertex.
        /// </summary>
        ///
        /// <returns> A ParentIterator. </returns>
        ParentIterator ParentEnd() const;

        /// <summary> An iterator used to expose the ChildPair values of the tree. </summary>
        using ChildrenIterator = std::vector<ChildPair>::const_iterator;

        /// <summary> Returns An iterator over ChildPair values that points to the first vertex. </summary>
        ///
        /// <returns> A ChildrenIterator. </returns>
        ChildrenIterator ChildrenBegin() const;

        /// <summary>
        /// Returns An iterator over ChildPair values that points beyond the last vertex.
        /// </summary>
        ///
        /// <returns> A ChildrenIterator. </returns>
        ChildrenIterator ChildrenEnd() const;

        /// <summary> An iterator used to expose the output values of the tree. </summary>
        using OutputIterator = std::vector<double>::const_iterator;

        /// <summary> Returns An iterator over output values that points to the first vertex. </summary>
        ///
        /// <returns> An OutputIterator. </returns>
        OutputIterator OutputBegin() const;

        /// <summary> Returns An iterator over output values that points beyond the last vertex. </summary>
        ///
        /// <returns> An OutputIterator. </returns>
        OutputIterator OutputEnd() const;

        /// <summary> Returns the number of vertices. </summary>
        ///
        /// <returns> The number vertices. </returns>
        int GetNumVertices() const;

        /// <summary> Returns the number of interior vertices. </summary>
        ///
        /// <returns> The number interior vertices. </returns>
        int GetNumInteriorVertices() const;

        /// <summary> Performs a split of a leaf. </summary>
        ///
        /// <param name="leaf_index"> Zero-based index of the leaf. </param>
        /// <param name="input_index"> Zero-based index of the input. </param>
        /// <param name="treshold"> The treshold. </param>
        /// <param name="child0_output"> The child 0 output. </param>
        /// <param name="child1_output"> The child 1 output. </param>
        void SplitLeaf(int leaf_index, 
                      int input_index, 
                      double treshold, 
                      double child0_output, 
                      double child1_output);

        /// <summary> Deleted copy constructor. </summary>
        DecisionTree(const DecisionTree&) = delete;

    private:
        
        std::vector<SplitRule> _splitRules;
        std::vector<ChildPair> _children;
        std::vector<int> _parents;
        std::vector<double> _outputs;
    };
}
