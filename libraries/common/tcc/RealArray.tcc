#include "..\include\RealArray.h"
// RealArray.tcc

namespace common
{
    template<typename RealType>
    bool RealArray<RealType>::Iterator::IsValid() const
    {
        return (_begin != _end);
    }

    template<typename RealType>
    void RealArray<RealType>::Iterator::Next()
    {
        ++_begin;
        ++_index;
        SkipZeros();
    }

    template<typename RealType>
    indexValue RealArray<RealType>::Iterator::Get() const
    {
        return indexValue{_index, (double)*_begin};
    }

    template<typename RealType>
    RealArray<RealType>::Iterator::Iterator(const StlIteratorType& begin, const StlIteratorType& end) : _begin(begin), _end(end), _index(0)
    {
        SkipZeros();
    }

    template<typename RealType>
    void RealArray<RealType>::Iterator::SkipZeros()
    {
        while(_begin < _end && *_begin == 0)
        {
            ++_begin;
            ++_index;
        }
    }

    template<typename RealType>
    RealArray<RealType>::RealArray(uint64 size) : vector<RealType>(size) {}

    template<typename RealType>
    template<typename IndexValueIteratorType, typename concept>
    RealArray<RealType>::RealArray(IndexValueIteratorType indexValueIterator) : vector<RealArray>(0)
    {
        while(indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            resize(entry.index+1);
            (*this)[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }
    
    template<typename RealType>
    typename RealArray<RealType>::Iterator RealArray<RealType>::GetIterator() const
    {
        return Iterator(cbegin(), cend());
    }

    template<typename RealType>
    void RealArray<RealType>::Print(ostream& os) const
    {
        for(RealType x : *this)
        {
            os << x << '\t';
        }
    }

    template<typename RealType>
    ostream& operator<<(ostream& os, const RealArray<RealType>& array)
    {
        array.Print(os);
    }
}