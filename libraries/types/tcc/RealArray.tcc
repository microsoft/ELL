// RealArray.tcc

namespace types
{
    //
    // SparseStlIterator implementation
    //
    template<typename ValueType>
    bool SparseStlIterator<ValueType>::IsValid() const
    {
        return (_begin != _end);
    }

    template<typename ValueType>
    void SparseStlIterator<ValueType>::Next()
    {
        ++_begin;
        ++_index;
        SkipZeros();
    }

    template<typename ValueType>
    IndexValue SparseStlIterator<ValueType>::Get() const
    {
        return IndexValue{ _index, (double)*_begin };
    }

    template<typename ValueType>
    SparseStlIterator<ValueType>::SparseStlIterator(const StlIteratorType& begin, const StlIteratorType& end) : _begin(begin), _end(end), _index(0)
    {
        SkipZeros();
    }

    template<typename ValueType>
    void SparseStlIterator<ValueType>::SkipZeros()
    {
        while (_begin < _end && *_begin == 0)
        {
            ++_begin;
            ++_index;
        }
    }

}
