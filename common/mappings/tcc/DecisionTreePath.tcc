// DecisionTreePath.tcc

namespace mappings
{
    template<typename ChildrenIteratorType>
    DecisionTreePath::DecisionTreePath(ChildrenIteratorType iter, ChildrenIteratorType end, int input_index_offset) : 
        _input_index_offset(input_index_offset)
    {
        _type = types::decisionTreePath;

        while (iter != end)
        {
            _children.emplace_back(iter->GetChild0(), iter->GetChild1());
            ++iter;
        }
    }

}