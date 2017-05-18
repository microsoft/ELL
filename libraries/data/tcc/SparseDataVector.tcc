////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

namespace ell
{
namespace data
{

    template <typename ElementType, typename IndexListType>
    SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::SparseDataVectorIterator(
        const IndexIteratorType& index_iterator, const ValueIteratorType& valueIterator, size_t size)
        : _indexIterator(index_iterator), _valueIterator(valueIterator), _size(size)
    {
    }
    template <typename ElementType, typename IndexListType>
    bool SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::IsValid() const 
    { 
        return _indexIterator.IsValid() && _indexIterator.Get() < _size; 
    }

    template <typename ElementType, typename IndexListType>
    void SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::Next() 
    { 
        _indexIterator.Next(); 
        ++_valueIterator; 
    }

    template <typename ElementType, typename IndexListType>
    IndexValue SparseDataVectorIterator<IterationPolicy::skipZeros, ElementType, IndexListType>::Get() const 
    { 
        return IndexValue{ _indexIterator.Get(), static_cast<double>(*_valueIterator) }; 
    }

    template <typename ElementType, typename IndexListType>
    void SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>::Next()
    {
        if (_index == _iteratorIndex)
        {
            _indexIterator.Next();
            ++_valueIterator;
            if (_indexIterator.IsValid() && _indexIterator.Get() < _size)
            {
                _iteratorIndex = _indexIterator.Get();
            } 
            else 
            {
                _iteratorIndex = _size;
            }
        }
        ++_index;
    }

    template <typename ElementType, typename IndexListType>
    IndexValue SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>::Get() const 
    { 
        if (_index == _iteratorIndex)
        {
            return IndexValue{ _index, static_cast<double>(*_valueIterator) };
        }
        return IndexValue{ _index, 0.0 };
    }

    template<typename ElementType, typename IndexListType>
    SparseDataVectorIterator<IterationPolicy::all, ElementType, IndexListType>::SparseDataVectorIterator(const IndexIteratorType& indexIterator, const ValueIteratorType& valueIterator, size_t size) :
        _indexIterator(indexIterator), _valueIterator(valueIterator), _size(size)
    {
        _iteratorIndex = _indexIterator.IsValid() ? _indexIterator.Get() : _size;
    }

    template <typename ElementType, typename IndexListType>
    template <typename SparseIteratorType, IsIndexValueIterator<SparseIteratorType> Concept>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(SparseIteratorType SparseIterator)
    {
        AppendElements(std::move(SparseIterator));
    }

    template<typename ElementType, typename IndexListType>
    template<IterationPolicy policy>
    auto SparseDataVector<ElementType, IndexListType>::GetIterator(size_t size) const -> Iterator<policy> 
    { 
        return Iterator<policy>(_indexList.GetIterator(), _values.cbegin(), size); 
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::vector<IndexValue> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename ElementType, typename IndexListType>
    SparseDataVector<ElementType, IndexListType>::SparseDataVector(std::vector<double> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename ElementType, typename IndexListType>
    void SparseDataVector<ElementType, IndexListType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        ElementType storedValue = static_cast<ElementType>(value);
        assert(storedValue - value <= 1.0e-6 && value - storedValue <= 1.0e-6);

        if (_indexList.Size() > 0)
        {
            if (index <= _indexList.Max())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
            }
        }

        _indexList.Append(index);
        _values.push_back(storedValue);
    }

    template <typename ElementType, typename IndexListType>
    size_t SparseDataVector<ElementType, IndexListType>::PrefixLength() const
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
}
}
