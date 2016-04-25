////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     SparseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename ValueType, typename IntegerListType>
    template<typename IndexValueIteratorType, typename concept>
    SparseDataVector<ValueType, IntegerListType>::SparseDataVector(IndexValueIteratorType IndexValueIterator)
    {
        while(IndexValueIterator.IsValid())
        {
            auto IndexValue = IndexValueIterator.Get();
            SparseDataVector<ValueType, IntegerListType>::AppendEntry(IndexValue.index, IndexValue.value); // explicit call to SparseDataVector<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            IndexValueIterator.Next();
        }
    }
}
