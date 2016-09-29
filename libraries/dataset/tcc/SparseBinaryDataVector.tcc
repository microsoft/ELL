////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseBinaryDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cassert>
#include "..\include\SparseBinaryDataVector.h"

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
    template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> Concept>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            SparseBinaryDataVectorBase<IntegerListType>::AppendElement(indexValue.index, indexValue.value); // explicit call to SparseBinaryDataVectorBase<ElementType>::AppendElement is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template<typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(std::initializer_list<linear::IndexValue> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            SparseBinaryDataVectorBase<IntegerListType>::AppendElement(current->index, current->value);
            ++current;
        }
    }

    template<typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(std::initializer_list<double> list)
    {
        auto current = list.begin();
        auto end = list.end();
        size_t index = 0;
        while(current < end)
        {
            SparseBinaryDataVectorBase<IntegerListType>::AppendElement(index, *current);
            ++current;
            ++index;
        }
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
    void SparseBinaryDataVectorBase<IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            p_other[iter.Get()] += scalar;
            iter.Next();
        }
    }
}
}
