// IterableArray.tcc

namespace common
{
    template<typename ValueType>
    bool IterableArray<ValueType>::Iterator::IsValid() const
    {
        return (_begin != _end);
    }

    template<typename ValueType>
    void IterableArray<ValueType>::Iterator::Next()
    {
        ++_begin;
        ++_index;
    }

    template<typename ValueType>
    ValueType IterableArray<ValueType>::Iterator::Get() const
    {
        return (double)*_begin;
    }

    template<typename ValueType>
    IterableArray<ValueType>::Iterator::Iterator(const StlIteratorType& begin, const StlIteratorType& end) : _begin(begin), _end(end)
    {
        SkipZeros();
    }

    template<typename ValueType>
    IterableArray<ValueType>::IterableArray(uint64 size) : vector<ValueType>(size) {}

    template<typename ValueType>
    uint64 IterableArray<ValueType>::Size() const
    {
        return size();
    }

    template<typename ValueType>
    typename IterableArray<ValueType>::Iterator IterableArray<ValueType>::GetIterator() const
    {
        return Iterator(cbegin(), cend());
    }

    template<typename ValueType>
    void IterableArray<ValueType>::Print(ostream& os) const
    {
        for(ValueType x : *this)
        {
            os << x << '\t';
        }
    }

    template<typename ValueType>
    ostream& operator<<(ostream& os, const IterableArray<ValueType>& array)
    {
        array.Print(os);
    }
}