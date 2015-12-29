// RealArray.tcc

namespace common
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
    RealArray<ValueType>::RealArray(uint64 size) : vector<ValueType>(size) {}

    template<typename ValueType>
    template<typename IndexValueIteratorType, typename concept>
    RealArray<ValueType>::RealArray(IndexValueIteratorType IndexValueIterator) : vector<RealArray>(0)
    {
        while(IndexValueIterator.IsValid())
        {
            auto entry = IndexValueIterator.Get();
            resize(entry.index+1);
            (*this)[entry.index] = entry.value;
            IndexValueIterator.Next();
        }
    }
    
    template<typename ValueType>
    uint64 RealArray<ValueType>::Size() const
    {
        return size();
    }

    template<typename ValueType>
    typename RealArray<ValueType>::Iterator RealArray<ValueType>::GetIterator() const
    {
        return Iterator(cbegin(), cend());
    }

    template<typename ValueType>
    void RealArray<ValueType>::Print(ostream& os) const
    {
        for(ValueType x : *this)
        {
            os << x << '\t';
        }
    }

    template<typename ValueType>
    ostream& operator<<(ostream& os, const RealArray<ValueType>& array)
    {
        array.Print(os);
    }
}