////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlIndexValueIterator.h (data)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace data
{
    template <typename IteratorType>
    StlIndexValueIterator<IterationPolicy::skipZeros, IteratorType>::StlIndexValueIterator(const IteratorType& begin, const IteratorType& end, size_t size)
        : _current(begin), _end(end), _size(size), _index(0)
    {
        SkipZeros();
    }

    template<typename IteratorType>
    void StlIndexValueIterator<IterationPolicy::skipZeros, IteratorType>::Next()
    {
        ++_current;
        ++_index;
        SkipZeros();
    }

    template <typename IteratorType>
    void StlIndexValueIterator<IterationPolicy::skipZeros, IteratorType>::SkipZeros()
    {
        while (_current < _end && *_current == 0)
        {
            ++_current;
            ++_index;
        }
    }

    template <typename IteratorType>
    StlIndexValueIterator<IterationPolicy::all, IteratorType>::StlIndexValueIterator(const IteratorType& begin, const IteratorType& end, size_t size)
        : _current(begin), _end(end), _size(size)
    {
    }

    template<typename IteratorType>
    void StlIndexValueIterator<IterationPolicy::all, IteratorType>::Next()
    {
        ++_index;
        if (_current < _end)
        {
            ++_current;
        }
    }

    template<typename IteratorType>
    IndexValue StlIndexValueIterator<IterationPolicy::all, IteratorType>::Get() const
    { 
        return _current < _end ? IndexValue{ _index, (double)*_current } : IndexValue{ _index, 0.0 }; 
    }

    //
    // Convenience function to create iterator
    //

    template <IterationPolicy policy, typename ElementType>
    VectorIndexValueIterator<policy, ElementType> MakeVectorIndexValueIterator(const std::vector<ElementType>& vector)
    {
        return VectorIndexValueIterator<policy, ElementType>(vector.cbegin(), vector.cend(), vector.size());
    }

    template <IterationPolicy policy, typename ElementType>
    VectorIndexValueIterator<policy, ElementType> MakeVectorIndexValueIterator(const std::vector<ElementType>& vector, size_t size)
    {
        return VectorIndexValueIterator<policy, ElementType>(vector.cbegin(), vector.cend(), size);
    }
}
}
