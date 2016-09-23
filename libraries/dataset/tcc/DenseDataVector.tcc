////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

// stl
#include <cassert>

namespace emll
{
namespace dataset
{
    template <typename ElementType>
    template <typename IndexValueIteratorType, typename concept>
    DenseDataVector<ElementType>::DenseDataVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            DenseDataVector<ElementType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to DenseDataVector<ElementType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector() : _numNonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<ElementType> data) : _numNonzeros(0), _data(std::move(data))
    {
        for (auto value : _data)
        {
            if (value != 0)
            {
                ++_numNonzeros;
            }
        }
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::initializer_list<linear::IndexValue> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            DenseDataVector<ElementType>::AppendEntry(current->index, current->value); // explicit call to DenseDataVector<ElementType>::AppendEntry is given to avoid virtual function call in Ctor
            ++current;
        }
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::initializer_list<size_t> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            DenseDataVector<ElementType>::AppendEntry(*current, 1.0); // explicit call to DenseDataVector<ElementType>::AppendEntry is given to avoid virtual function call in Ctor
            ++current;
        }
    }

    template <typename ElementType>
    double DenseDataVector<ElementType>::operator[](size_t index) const
    {
        if (index >= _data.size())
        {
            return 0.0;
        }
        return static_cast<double>(_data[index]);
    }

    template <typename ElementType>
    void DenseDataVector<ElementType>::AppendEntry(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        ElementType storedValue = static_cast<ElementType>(value);
        assert(storedValue - value <= 1.0e-8 && value - storedValue <= 1.0e-8);
            
        if(index < _data.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
        }

        _data.resize(index + 1);
        _data[index] = storedValue;
        ++_numNonzeros;
    }
}
}
