////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseBinaryDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseBinaryDataVector.h"
#include "CompressedIntegerList.h"
#include "IntegerList.h"

// stl
#include <cassert>

namespace emll
{
namespace dataset
{
    template <typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::Iterator::Iterator(const IndexIteratorType& list_iterator)
        : _list_iterator(list_iterator)
    {
    }

    template <typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::AppendEntry(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(value == 1);

        _indices.Append(index);
    }

    template <typename IntegerListType>
    size_t SparseBinaryDataVectorBase<IntegerListType>::Size() const
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
    void SparseBinaryDataVectorBase<IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            p_other[iter.Get()] += scalar;
            iter.Next();
        }
    }

    template <typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Dot(const double* p_other) const
    {
        double value = 0.0;

        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            value += (double)p_other[iter.Get()];
            iter.Next();
        }

        return value;
    }

    template <typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Print(std::ostream& os) const
    {
        auto iterator = GetIterator();
        while (iterator.IsValid())
        {
            auto entry = iterator.Get();
            os << entry.index << ':' << entry.value << '\t';
            iterator.Next();
        }
    }

    // Explicit instantiation
    template class SparseBinaryDataVectorBase<utilities::CompressedIntegerList>;
    template class SparseBinaryDataVectorBase<utilities::IntegerList>;
}
}
