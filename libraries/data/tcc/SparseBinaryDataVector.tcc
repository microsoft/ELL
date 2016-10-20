////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseBinaryDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

// stl
#include <cassert>

namespace emll
{
namespace data
{
    template <typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::Iterator::Iterator(const IndexIteratorType& list_iterator)
        : _list_iterator(list_iterator)
    {
    }

    template <typename IntegerListType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        AppendElements(std::move(indexValueIterator));
    }

    template <typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(value == 1);

        _indices.Append(index);
    }

    template <typename IntegerListType>
    size_t SparseBinaryDataVectorBase<IntegerListType>::PrefixLength() const
    {
        if (_indices.Size() == 0)
        {
            return 0;
        }
        else
        {
            return _indices.Max() + 1;
        }
    }

    template <typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Dot(const math::UnorientedConstVectorReference<double>& vector) const
    {
        double value = 0.0;

        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            value += vector[iter.Get()];
            iter.Next();
        }

        return value;
    }

    template <typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::AddTo(math::RowVectorReference<double>& vector, double scalar) const
    {
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            vector[iter.Get()] += scalar;
            iter.Next();
        }
    }
}
}
