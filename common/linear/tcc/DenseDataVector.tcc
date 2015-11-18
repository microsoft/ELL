// DenseDataVector.tcc

namespace linear
{
    template<typename ValueType>
    template<typename IndexValueIteratorType, typename concept>
    DenseDataVector<ValueType>::DenseDataVector(IndexValueIteratorType&& indexValueIterator)
    {
        // TODO, check that IndexValueIteratorType is the right type by adding a using statement to all of the iter defitniions

        while(indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.GetValue();
            PushBack(indexValue.GetIndex(), indexValue.GetValue());
            indexValueIterator.Next();
        }
    }
}
