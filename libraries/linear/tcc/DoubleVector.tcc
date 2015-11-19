// DoubleVector.tcc

namespace linear
{
    template<typename IndexValueIteratorType, typename concept>
    DoubleVector::DoubleVector(IndexValueIteratorType indexValueIterator)
    {
        while(indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            PushBack(indexValue.index, indexValue.value);
            indexValueIterator.Next();
        }
    }
}


