////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIndexValueIteratorAdapter.h (types)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace types
{
    //
    // StlIndexValueIteratorAdapter implementation
    //
    template<typename ValueType>
    bool StlIndexValueIteratorAdapter<ValueType>::IsValid() const
    {
        return (_begin != _end);
    }

    template<typename ValueType>
    void StlIndexValueIteratorAdapter<ValueType>::Next()
    {
        ++_begin;
        ++_index;
        SkipZeros();
    }

    template<typename ValueType>
    IndexValue StlIndexValueIteratorAdapter<ValueType>::Get() const
    {
        return IndexValue{ _index, (double)*_begin };
    }

    template<typename ValueType>
    StlIndexValueIteratorAdapter<ValueType>::StlIndexValueIteratorAdapter(const StlIteratorType& begin, const StlIteratorType& end) : _begin(begin), _end(end), _index(0)
    {
        SkipZeros();
    }

    template<typename ValueType>
    void StlIndexValueIteratorAdapter<ValueType>::SkipZeros()
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
    StlIndexValueIteratorAdapter<ValueType> inline MakeStlIndexValueIteratorAdapter(const std::vector<ValueType>& arr)
    {
        return StlIndexValueIteratorAdapter<ValueType>(arr.cbegin(), arr.cend());
    }


}
