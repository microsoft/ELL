////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseBinaryDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
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

    template <typename IntegerListType>
    std::unique_ptr<IDataVector> SparseBinaryDataVectorBase<IntegerListType>::Clone() const
    {
        auto ptr = std::make_unique<SparseBinaryDataVectorBase<IntegerListType>>(*this); 
        return std::move(ptr);
    }


    template <typename IntegerListType>
    IDataVector::type SparseBinaryDataVectorBase<IntegerListType>::GetType() const
    {
        return type::none; // TODO: Find a way to avoid requiring GetType() to be defined in non-concrete classes. We could just move the Clone method to the concrete classes.
    }
}
