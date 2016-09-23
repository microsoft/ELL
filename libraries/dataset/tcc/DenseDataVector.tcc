////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"
#include "..\include\DenseDataVector.h"

namespace emll
{
namespace dataset
{
    template <typename ValueType>
    template <typename IndexValueIteratorType, typename concept>
    DenseDataVector<ValueType>::DenseDataVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            DenseDataVector<ValueType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to DenseDataVector<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector()
        : _numNonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template <typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector(std::vector<ValueType> data)
        : _numNonzeros(0), _data(std::move(data))
    {
        for (auto value : _data)
        {
            if (value != 0)
            {
                ++_numNonzeros;
            }
        }
    }

    template<typename ValueType>
    inline DenseDataVector<ValueType>::DenseDataVector(std::initializer_list<linear::IndexValue> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            _data.resize(current->index+1);
            _data[current->index] = current->value;
            ++current;
        }
    }

    template <typename ValueType>
    double DenseDataVector<ValueType>::operator[](size_t index) const
    {
        if (index >= _data.size())
        {
            return 0.0;
        }
        return (double)_data[index];
    }

    template <typename ValueType>
    void DenseDataVector<ValueType>::AppendEntry(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        if(index < _data.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
        }

        _data.resize(index + 1);
        _data[index] = (ValueType)value;
        ++_numNonzeros;
    }
}
}
