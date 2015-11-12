// DecisionTreePath.tcc

namespace mappings
{
	template<typename ChildrenIteratorType>
	DecisionTreePath::DecisionTreePath(ChildrenIteratorType iter, ChildrenIteratorType end, int input_index_offset) : 
		_input_index_offset(input_index_offset)
	{
		_type = types::DecisionTreePath;

		while (iter != end)
		{
			_children.emplace_back(iter->get_child0(), iter->get_child1());
			++iter;
		}
	}

}