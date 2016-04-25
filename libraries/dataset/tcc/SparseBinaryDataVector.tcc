////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     SparseBinaryDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename IntegerListType>
    template<typename IndexValueIteratorType, typename concept>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(IndexValueIteratorType IndexValueIterator)
    {
        while(IndexValueIterator.IsValid())
        {
            auto IndexValue = IndexValueIterator.Get();
            SparseBinaryDataVectorBase<IntegerListType>::AppendEntry(IndexValue.index, IndexValue.value); // explicit call to SparseBinaryDataVectorBase<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            IndexValueIterator.Next();
        }
    }
}
