// DenseDataVector.tcc

namespace dataset
{
    template<typename ValueType>
    template<typename IndexValueIteratorType, typename concept>
    DenseDataVector<ValueType>::DenseDataVector(IndexValueIteratorType IndexValueIterator) 
    {
        while(IndexValueIterator.IsValid())
        {
            auto IndexValue = IndexValueIterator.Get();
            PushBack(IndexValue.index, IndexValue.value);
            IndexValueIterator.Next();
        }
    }
}
