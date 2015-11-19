// SparseBinaryDataVector.tcc

namespace dataset
{
    template<typename IntegerListType>
    template<typename IndexValueIteratorType, typename concept>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        while(indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            PushBack(indexValue.index, indexValue.value);
            indexValueIterator.Next();
        }
    }
}