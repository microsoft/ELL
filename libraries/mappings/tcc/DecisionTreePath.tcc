// DecisionTreePath.tcc

namespace mappings
{
    template<typename ChildrenIteratorType>
    DecisionTreePath::DecisionTreePath(ChildrenIteratorType iter, ChildrenIteratorType end, uint64 inputIndexOffset) : 
        _inputIndexOffset(inputIndexOffset)
    {
        _type = Types::decisionTreePath;

        while (iter != end)
        {
            _children.emplace_back(iter->GetChild0(), iter->GetChild1());
            ++iter;
        }
    }

}