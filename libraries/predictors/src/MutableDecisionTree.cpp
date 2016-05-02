////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MutableDecisionTree.cpp (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MutableDecisionTree.h"

// stl
#include <stdexcept>

namespace decisionTree
{
    MutableDecisionTree::SplitRule::SplitRule(int index, double value) :
        _index(index), _value(value)
    {}

    int MutableDecisionTree::SplitRule::GetIndex() const
    {
        return _index;
    }

    double MutableDecisionTree::SplitRule::Get() const
    {
        return _value;
    }

    MutableDecisionTree::ChildPair::ChildPair(int child0, int child1) :
        _child0(child0), _child1(child1)
    {}

    int MutableDecisionTree::ChildPair::GetChild0() const
    {
        return _child0;
    }

    int MutableDecisionTree::ChildPair::GetChild1() const
    {
        return _child1;
    }

    void MutableDecisionTree::ChildPair::SetChild0(int index)
    {
        _child0 = index;
    }

    void MutableDecisionTree::ChildPair::SetChild1(int index)
    {
        _child1 = index;
    }

    MutableDecisionTree::MutableDecisionTree(double root_output) :
        _parents(1), _outputs(1)
    {
        _parents[0] = -1;
        _outputs[0] = root_output;
    }

    MutableDecisionTree::SplitRuleIterator MutableDecisionTree::SplitRuleBegin() const
    {
        return _splitRules.cbegin();
    }

    MutableDecisionTree::SplitRuleIterator MutableDecisionTree::SplitRuleEnd() const
    {
        return _splitRules.cend();
    }

    MutableDecisionTree::ParentIterator MutableDecisionTree::ParentBegin() const
    {
        return _parents.cbegin();
    }

    MutableDecisionTree::ParentIterator MutableDecisionTree::ParentEnd() const
    {
        return _parents.cend();
    }

    MutableDecisionTree::ChildrenIterator MutableDecisionTree::ChildrenBegin() const
    {
        return _children.cbegin();
    }

    MutableDecisionTree::ChildrenIterator MutableDecisionTree::ChildrenEnd() const
    {
        return _children.cend();
    }

    MutableDecisionTree::OutputIterator MutableDecisionTree::OutputBegin() const
    {
        return _outputs.cbegin();
    }

    MutableDecisionTree::OutputIterator MutableDecisionTree::OutputEnd() const
    {
        return _outputs.cend();
    }

    int MutableDecisionTree::GetNumVertices() const
    {
        return (int)_parents.size();
    }

    int MutableDecisionTree::GetNumInteriorVertices() const
    {
        return (int)_splitRules.size();
    }

    void MutableDecisionTree::SplitLeaf(
        int leaf_index,
        int input_index,
        double threshold,
        double child0_output,
        double child1_output)
    {

        // check that the arguments are valid
        if(leaf_index >= GetNumVertices() || leaf_index < GetNumInteriorVertices())
        {
            throw std::runtime_error("leaf index out of bounds");
        }

        // get new indices
        int new_interior_index = GetNumInteriorVertices();
        int child0_index = GetNumVertices();
        int child1_index = child0_index + 1;

        // std::swap leaf_index with new_interior_index
        if (leaf_index != new_interior_index)
        {

            // tell the parents about the std::swap
            int parent_of_leaf = _parents[leaf_index];
            int parent_of_new_interior = _parents[new_interior_index];

            if (_children[parent_of_leaf].GetChild0() == leaf_index)
            {
                _children[parent_of_leaf].SetChild0(new_interior_index);
            }
            else
            {
                _children[parent_of_leaf].SetChild1(new_interior_index);
            }
            if (_children[parent_of_new_interior].GetChild0() == new_interior_index)
            {
                _children[parent_of_new_interior].SetChild0(leaf_index);
            }
            else
            {
                _children[parent_of_new_interior].SetChild1(leaf_index);
            }
            
            // std::swap the vertices
            std::swap(_parents[leaf_index], _parents[new_interior_index]);
            std::swap(_outputs[leaf_index], _outputs[new_interior_index]);
        }

        // Add new leaves
        _outputs.push_back(child0_output);
        _outputs.push_back(child1_output);
        _parents.push_back(new_interior_index);
        _parents.push_back(new_interior_index);

        // Add new interior node
        _children.emplace_back(child0_index, child1_index);
        _splitRules.emplace_back(input_index, threshold);
    }
}
