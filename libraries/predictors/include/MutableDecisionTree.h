////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     MutableDecisionTree.h (predictors)
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
    /// Represents a growable binary decision tree with interval splitting rules and outputs in all vertices
    ///
    class MutableDecisionTree
    {

    public:
        
        /// Represents an index value pair, used to store the split rules
        ///
        class SplitRule
        {
        public:

            /// Constructs an index value
            ///
            SplitRule(int index, double value);

            /// \return The index
            ///
            int GetIndex() const;

            /// \returns The value
            ///
            double Get() const;

        private:
            int _index;
            double _value;
        };

        /// Represents a pair of Children of a binary tree
        ///
        class Children // TODO rename to ChildPair
        {
        public:

            /// Constructs the pair of Children
            ///
            Children(int child0, int child1);

            /// \returns Child 0
            ///
            int GetChild0() const;

            /// \returns Child 1
            ///
            int GetChild1() const;

            /// Sets child 0
            ///
            void SetChild0(int index);

            /// Sets child 1
            ///
            void SetChild1(int index);

        private:
            int _child0;
            int _child1;
        };

        /// Constructs an empty mutable decision tree with one leaf, the root. 
        ///
        MutableDecisionTree(double root_output = 0.0);

        /// An iterator used to expose the tree split rules
        ///
        using SplitRuleIterator = std::vector<SplitRule>::const_iterator;

        /// \returns An iterator over the split rules that points to the first SplitRule
        ///
        SplitRuleIterator SplitRuleBegin() const;

        /// \returns An iterator over the split rules that points beyond the last SplitRule
        ///
        SplitRuleIterator SplitRuleEnd() const;

        /// An iterator used to expose the parent identities of vertices in the the tree
        ///
        using ParentIterator = std::vector<int>::const_iterator;

        /// \returns An iterator over parent indentities that points to the first vertex
        ///
        ParentIterator ParentBegin() const;

        /// \returns An iterator over parent identities that points beyond the last vertex
        ///
        ParentIterator ParentEnd() const;

        /// An iterator used to expose the Children values of the tree
        ///
        using ChildrenIterator = std::vector<Children>::const_iterator;

        /// \returns An iterator over Children values that points to the first vertex
        ///
        ChildrenIterator ChildrenBegin() const;

        /// \returns An iterator over Children values that points beyond the last vertex
        ///
        ChildrenIterator ChildrenEnd() const;

        /// An iterator used to expose the output values of the tree
        ///
        using OutputIterator = std::vector<double>::const_iterator;

        /// \returns An iterator over output values that points to the first vertex
        ///
        OutputIterator OutputBegin() const;

        /// \returns An iterator over output values that points beyond the last vertex
        ///
        OutputIterator OutputEnd() const;

        /// \returns The number of vertices
        ///
        int GetNumVertices() const;

        /// \returns The number of interior vertices
        ///
        int GetNumInteriorVertices() const;

        /// Performs a split of a leaf
        ///
        void SplitLeaf(int leaf_index, 
                      int input_index, 
                      double treshold, 
                      double child0_output, 
                      double child1_output);

        MutableDecisionTree(const MutableDecisionTree&) = delete;

    private:
        
        std::vector<SplitRule> _splitRules;
        std::vector<Children> _children;
        std::vector<int> _parents;
        std::vector<double> _outputs;
    };
}
