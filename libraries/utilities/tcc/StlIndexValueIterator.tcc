////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StlIndexValueIteratorAdapter.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace utilities
{
    //
    // StlIndexValueIterator implementation
    //
    template <typename IteratorType, typename ValueType>
    StlIndexValueIterator<IteratorType, ValueType>::StlIndexValueIterator(const IteratorType& begin, const IteratorType& end)
        : _begin(begin), _end(end), _index(0)
    {
        SkipZeros();
    }

    template <typename IteratorType, typename ValueType>
    void StlIndexValueIterator<IteratorType, ValueType>::Next()
    {
        ++_begin;
        ++_index;
        SkipZeros();
    }

    template <typename IteratorType, typename ValueType>
    void StlIndexValueIterator<IteratorType, ValueType>::SkipZeros()
    {
        while (_begin < _end && *_begin == 0)
        {
            ++_begin;
            ++_index;
        }
    }

    //
    // Convenience function to create iterator
    //
    template <typename ValueType>
    VectorIndexValueIterator<ValueType> inline MakeStlIndexValueIterator(const std::vector<ValueType>& arr)
    {
        return VectorIndexValueIterator<ValueType>(arr.cbegin(), arr.cend());
    }

    // specialization for IndexValueIterators
    template <typename ValueType, typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType>>
    void CopyToArray(IndexValueIteratorType& indexValueIterator, std::vector<ValueType>& array)
    {
        std::fill(array.begin(), array.end(), 0);
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            if (entry.index >= array.size())
            {
                array.resize(entry.index + 1);
            }
            array[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }
}
}
