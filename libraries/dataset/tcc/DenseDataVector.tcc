////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
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
            DenseDataVector<ValueType>::AppendEntry(IndexValue.index, IndexValue.value); // explicit call to DenseDataVector<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            IndexValueIterator.Next();
        }
    }

    template <typename ValueType>
    std::unique_ptr<IDataVector> DenseDataVector<ValueType>::Clone() const
    {
        DenseDataVector<ValueType> result = *this;
        return std::make_unique<DenseDataVector<ValueType>>(std::move(result)); 
    }
}
