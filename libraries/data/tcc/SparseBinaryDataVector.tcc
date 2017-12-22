////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseBinaryDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

// stl
#include <cassert>

namespace ell
{
namespace data
{
    template <typename IndexListType>
    SparseBinaryDataVectorIterator<IterationPolicy::skipZeros, IndexListType>::SparseBinaryDataVectorIterator(const IndexIteratorType& listIterator, size_t size)
        : _indexIterator(listIterator), _size(size)
    {
    }

    template<typename IndexListType>
    void SparseBinaryDataVectorIterator<IterationPolicy::all, IndexListType>::Next()
    {
        if (_index == _iteratorIndex)
        {
            _indexIterator.Next();
            _iteratorIndex = _indexIterator.IsValid() ? _indexIterator.Get() : _size;
        }
        ++_index;
    }

    template<typename IndexListType>
    IndexValue SparseBinaryDataVectorIterator<IterationPolicy::all, IndexListType>::Get() const
    { 
        return _index == _iteratorIndex ? IndexValue{ _index, 1.0 } : IndexValue{ _index, 0.0 }; 
    }

    template<typename IndexListType>
    SparseBinaryDataVectorIterator<IterationPolicy::all, IndexListType>::SparseBinaryDataVectorIterator(const IndexIteratorType & listIterator, size_t size) :
        _indexIterator(listIterator), _size(size)
    {
        _iteratorIndex = _indexIterator.IsValid() ? _indexIterator.Get() : _size;
    }

    template <typename IndexListType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    SparseBinaryDataVectorBase<IndexListType>::SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        AppendElements(std::move(indexValueIterator));
    }

    template<typename IndexListType>
    template<IterationPolicy policy>
    auto SparseBinaryDataVectorBase<IndexListType>::GetIterator(size_t size) const -> Iterator<policy>
    { 
        return Iterator<policy>(_indexList.GetIterator(), size); 
    }

    template<typename IndexListType>
    template<IterationPolicy policy>
    auto SparseBinaryDataVectorBase<IndexListType>::GetIterator() const -> Iterator<policy>
    {
        return GetIterator<policy>(PrefixLength()); 
    }

    template <typename IndexListType>
    SparseBinaryDataVectorBase<IndexListType>::SparseBinaryDataVectorBase(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename IndexListType>
    SparseBinaryDataVectorBase<IndexListType>::SparseBinaryDataVectorBase(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename IndexListType>
    SparseBinaryDataVectorBase<IndexListType>::SparseBinaryDataVectorBase(std::vector<IndexValue> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename IndexListType>
    SparseBinaryDataVectorBase<IndexListType>::SparseBinaryDataVectorBase(std::vector<double> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename IndexListType>
    void SparseBinaryDataVectorBase<IndexListType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(value == 1);

        _indexList.Append(index);
    }

    template <typename IndexListType>
    size_t SparseBinaryDataVectorBase<IndexListType>::PrefixLength() const
    {
        if (_indexList.Size() == 0)
        {
            return 0;
        }
        else
        {
            return _indexList.Max() + 1;
        }
    }

    template <typename IndexListType>
    double SparseBinaryDataVectorBase<IndexListType>::Dot(math::UnorientedConstVectorBase<double> vector) const
    {
        double value = 0.0;

        auto iter = _indexList.GetIterator();
        while (iter.IsValid())
        {
            value += vector[iter.Get()];
            iter.Next();
        }

        return value;
    }

    template <typename IndexListType>
    void SparseBinaryDataVectorBase<IndexListType>::AddTo(math::RowVectorReference<double> vector) const
    {
        auto iter = _indexList.GetIterator();
        auto size = vector.Size();

        while (iter.IsValid())
        {
            auto index = iter.Get();
            if (index >= size)
            {
                return;
            }

            vector[index] += 1.0;
            iter.Next();
        }
    }
}
}
