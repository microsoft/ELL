////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StlIndexValueIteratorAdapter.h (data)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace data
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
    VectorIndexValueIterator<ValueType> MakeVectorIndexValueIterator(const std::vector<ValueType>& arr)
    {
        return VectorIndexValueIterator<ValueType>(arr.cbegin(), arr.cend());
    }
}
}
