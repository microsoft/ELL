#include "..\include\DenseDataVector.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
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
    DenseDataVector<ValueType>::DenseDataVector(IndexValueIteratorType indexValueIterator) 
    {
        while(indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            DenseDataVector<ValueType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to DenseDataVector<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename ValueType>
    std::unique_ptr<IDataVector> DenseDataVector<ValueType>::Clone() const
    {
        DenseDataVector<ValueType> result = *this;
        return std::make_unique<DenseDataVector<ValueType>>(std::move(result)); 
    }
    
    template<typename ValueType>
    std::vector<double> DenseDataVector<ValueType>::ToArray(uint64_t size) const
    {
        auto vector = std::vector<double>(size);
        if(size > _data.size())
        {
            size = _data.size();
        }
        std::copy(_data.cbegin(), _data.cbegin()+size, vector.begin());
        return vector;
    }

    template <typename ValueType>
    IDataVector::type DenseDataVector<ValueType>::GetType() const
    {
        return type::none; // In order for Clone to work, we must implement GetType in the base class (or we could do it with a constexpr function)
    }
}
