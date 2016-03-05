// RealArray.tcc

namespace types
{
    template<typename ValueType>
    bool RealArray<ValueType>::Iterator::IsValid() const
    {
        return (_begin != _end);
    }

    template<typename ValueType>
    void RealArray<ValueType>::Iterator::Next()
    {
        ++_begin;
        ++_index;
        SkipZeros();
    }

    template<typename ValueType>
    IndexValue RealArray<ValueType>::Iterator::Get() const
    {
        return IndexValue{_index, (double)*_begin};
    }

    template<typename ValueType>
    RealArray<ValueType>::Iterator::Iterator(const StlIteratorType& begin, const StlIteratorType& end) : _begin(begin), _end(end), _index(0)
    {
        SkipZeros();
    }

    template<typename ValueType>
    void RealArray<ValueType>::Iterator::SkipZeros()
    {
        while(_begin < _end && *_begin == 0)
        {
            ++_begin;
            ++_index;
        }
    }

    template<typename ValueType>
    RealArray<ValueType>::RealArray(uint64 size) : std::vector<ValueType>(size) {}

    template<typename ValueType>
    typename RealArray<ValueType>::Iterator RealArray<ValueType>::GetIterator() const
    {
        return Iterator(this->cbegin(), this->cend());
    }
}
