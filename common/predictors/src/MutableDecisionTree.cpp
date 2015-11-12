// MutableDecisionTree.cpp

#include "MutableDecisionTree.h"
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::swap;

namespace decision_tree
{
	MutableDecisionTree::SplitRule::SplitRule(int index, double value) :
		_index(index), _value(value)
	{}

	int MutableDecisionTree::SplitRule::GetIndex() const
	{
		return _index;
	}

	double MutableDecisionTree::SplitRule::GetValue() const
	{
		return _value;
	}

	MutableDecisionTree::Children::Children(int child0, int child1) :
		_child0(child0), _child1(child1)
	{}

	int MutableDecisionTree::Children::get_child0() const
	{
		return _child0;
	}

	int MutableDecisionTree::Children::get_child1() const
	{
		return _child1;
	}

	void MutableDecisionTree::Children::set_child0(int index)
	{
		_child0 = index;
	}

	void MutableDecisionTree::Children::set_child1(int index)
	{
		_child1 = index;
	}

	MutableDecisionTree::MutableDecisionTree(double root_output) :
		_parents(1), _outputs(1)
	{
		_parents[0] = -1;
		_outputs[0] = root_output;
	}

	MutableDecisionTree::split_rule_iterator MutableDecisionTree::SplitRuleBegin() const
	{
		return _split_rules.cbegin();
	}

	MutableDecisionTree::split_rule_iterator MutableDecisionTree::SplitRuleEnd() const
	{
		return _split_rules.cend();
	}

	MutableDecisionTree::parent_iterator MutableDecisionTree::ParentBegin() const
	{
		return _parents.cbegin();
	}

	MutableDecisionTree::parent_iterator MutableDecisionTree::ParentEnd() const
	{
		return _parents.cend();
	}

	MutableDecisionTree::children_iterator MutableDecisionTree::ChildrenBegin() const
	{
		return _children.cbegin();
	}

	MutableDecisionTree::children_iterator MutableDecisionTree::ChildrenEnd() const
	{
		return _children.cend();
	}

	MutableDecisionTree::output_iterator MutableDecisionTree::OutputBegin() const
	{
		return _outputs.cbegin();
	}

	MutableDecisionTree::output_iterator MutableDecisionTree::OutputEnd() const
	{
		return _outputs.cend();
	}

	int MutableDecisionTree::GetNumVertices() const
	{
		return (int)_parents.size();
	}

	int MutableDecisionTree::GetNumInteriorVertices() const
	{
		return (int)_split_rules.size();
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
			throw runtime_error("leaf index out of bounds");
		}

		// get new indices
		int new_interior_index = GetNumInteriorVertices();
		int child0_index = GetNumVertices();
		int child1_index = child0_index + 1;

		// swap leaf_index with new_interior_index
		if (leaf_index != new_interior_index)
		{

			// tell the parents about the swap
			int parent_of_leaf = _parents[leaf_index];
			int parent_of_new_interior = _parents[new_interior_index];

			if (_children[parent_of_leaf].get_child0() == leaf_index)
			{
				_children[parent_of_leaf].set_child0(new_interior_index);
			}
			else
			{
				_children[parent_of_leaf].set_child1(new_interior_index);
			}
			if (_children[parent_of_new_interior].get_child0() == new_interior_index)
			{
				_children[parent_of_new_interior].set_child0(leaf_index);
			}
			else
			{
				_children[parent_of_new_interior].set_child1(leaf_index);
			}
			
			// swap the vertices
			swap(_parents[leaf_index], _parents[new_interior_index]);
			swap(_outputs[leaf_index], _outputs[new_interior_index]);
		}

		// Add new leaves
		_outputs.push_back(child0_output);
		_outputs.push_back(child1_output);
		_parents.push_back(new_interior_index);
		_parents.push_back(new_interior_index);

		// Add new interior node
		_children.emplace_back(child0_index, child1_index);
		_split_rules.emplace_back(input_index, threshold);
	}
}
