////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DenseDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

// stl
#include <cassert>

namespace ell
{
namespace data
{
    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector()
        : _numNonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template <typename ElementType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    DenseDataVector<ElementType>::DenseDataVector(IndexValueIteratorType indexValueIterator)
    {
        AppendElements(std::move(indexValueIterator));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType>
    DenseDataVector<ElementType>::DenseDataVector(std::vector<float> list)
    {
        AppendElements(std::move(list));
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

    template<typename ElementType>
    template<IterationPolicy policy>
    VectorIndexValueIterator<policy, ElementType> DenseDataVector<ElementType>::GetIterator(size_t size) const
    { 
        return MakeVectorIndexValueIterator<policy>(_data, size); 
    }

    template<typename ElementType> // move this to datavectorbase?
    template<IterationPolicy policy>
    VectorIndexValueIterator<policy, ElementType> DenseDataVector<ElementType>::GetIterator() const
    {
        return GetIterator<policy>(PrefixLength());
    }

    template <typename ElementType>
    void DenseDataVector<ElementType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        ElementType storedValue = static_cast<ElementType>(value);
        assert(storedValue - value <= 1.0e-5 && value - storedValue <= 1.0e-5);

        if (index < _data.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
        }

        _data.resize(index + 1);
        _data[index] = storedValue;
        ++_numNonzeros;
    }
}
}
