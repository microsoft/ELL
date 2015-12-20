// IterableArray.tcc

namespace common
{
    template<typename ValueType>
    VectorIterator<ValueType>::VectorIterator(const StlVectorIteratorType& begin, const StlVectorIteratorType& end) : _begin(begin), _end(end)
    {}

    template<typename ValueType>
    bool VectorIterator<ValueType>::IsValid() const
    {
        return (_begin != _end);
    }

    template<typename ValueType>
    void VectorIterator<ValueType>::Next()
    {
        ++_begin;
    }

    template<typename ValueType>
    ValueType VectorIterator<ValueType>::Get() const
    {
        return *_begin;
    }
}