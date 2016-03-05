// RealArray.tcc

namespace types
{
    //
    // RealArray implementation
    //
    template<typename ValueType>
    RealArray<ValueType>::RealArray(uint64 size) : std::vector<ValueType>(size) {}

    template<typename ValueType>
    typename RealArray<ValueType>::Iterator RealArray<ValueType>::GetIterator() const
    {
        return SparseStlIterator<ValueType>(this->cbegin(), this->cend());
    }

    template<typename ValueType>
    ValueType* RealArray<ValueType>::GetDataPointer()
    {
        return data();
    }

    template<typename ValueType>
    const ValueType* RealArray<ValueType>::GetDataPointer() const
    {
        return data();
    }

    //
    //
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
