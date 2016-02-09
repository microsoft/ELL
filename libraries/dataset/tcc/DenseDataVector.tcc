////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DenseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename ValueType>
    template<typename IndexValueIteratorType, typename concept>
    DenseDataVector<ValueType>::DenseDataVector(IndexValueIteratorType IndexValueIterator) 
    {
        while(IndexValueIterator.IsValid())
        {
            auto IndexValue = IndexValueIterator.Get();
            DenseDataVector<ValueType>::PushBack(IndexValue.index, IndexValue.value); // explicit call to DenseDataVector<ValueType>::PushBack is given to avoid virtual function call in Ctor
            IndexValueIterator.Next();
        }
    }
}
