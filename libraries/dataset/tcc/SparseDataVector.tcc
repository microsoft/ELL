// SparseDataVector.tcc

namespace dataset
{
    template<typename ValueType, typename IntegerListType>
    template<typename IndexValueIteratorType, typename concept>
    SparseDataVector<ValueType, IntegerListType>::SparseDataVector(IndexValueIteratorType&& indexValueIterator)
    {
        while(indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            PushBack(indexValue.index, indexValue.value);
            indexValueIterator.Next();
        }
    }
}
