////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlStridedIterator.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename IteratorType>
    StlStridedIterator<IteratorType>::StlStridedIterator(const IteratorType& iterator, difference_type stride)
        : _iterator(iterator), _stride(stride)
    {
    }

    //
    // Required operations for random-access iterators:
    //

    template <typename IteratorType>
    bool StlStridedIterator<IteratorType>::operator==(const StlStridedIterator<IteratorType>& other) const
    {
        return _iterator == other._iterator;
    }

    template <typename IteratorType>
    bool StlStridedIterator<IteratorType>::operator!=(const StlStridedIterator<IteratorType>& other) const
    {
        return _iterator != other._iterator;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::reference StlStridedIterator<IteratorType>::operator*()
    {
        return *_iterator;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::const_reference StlStridedIterator<IteratorType>::operator*() const
    {
        return *_iterator;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::pointer StlStridedIterator<IteratorType>::operator->()
    {
        return _iterator;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::const_pointer StlStridedIterator<IteratorType>::operator->() const
    {
        return _iterator;
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType>& StlStridedIterator<IteratorType>::operator++()
    {
        _iterator += _stride;
        return *this;
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType> StlStridedIterator<IteratorType>::operator++(int)
    {
        auto temp = _iterator;
        _iterator += _stride;
        return { temp, _stride };
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType>& StlStridedIterator<IteratorType>::operator--()
    {
        _iterator -= _stride;
        return *this;
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType> StlStridedIterator<IteratorType>::operator--(int)
    {
        auto temp = _iterator;
        _iterator -= _stride;
        return { temp, _stride };
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType>& StlStridedIterator<IteratorType>::operator+=(difference_type increment)
    {
        _iterator += increment * _stride;
        return *this;
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType>& StlStridedIterator<IteratorType>::operator-=(difference_type increment)
    {
        _iterator -= increment * _stride;
        return *this;
    }

    template <typename IteratorType>
    bool StlStridedIterator<IteratorType>::operator>(const StlStridedIterator<IteratorType>& other) const
    {
        return _iterator > other._iterator;
    }

    template <typename IteratorType>
    bool StlStridedIterator<IteratorType>::operator<(const StlStridedIterator<IteratorType>& other) const
    {
        return _iterator < other._iterator;
    }

    template <typename IteratorType>
    bool StlStridedIterator<IteratorType>::operator>=(const StlStridedIterator<IteratorType>& other) const
    {
        return _iterator >= other._iterator;
    }

    template <typename IteratorType>
    bool StlStridedIterator<IteratorType>::operator<=(const StlStridedIterator<IteratorType>& other) const
    {
        return _iterator <= other._iterator;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::reference StlStridedIterator<IteratorType>::operator[](typename StlStridedIterator<IteratorType>::size_type index)
    {
        return _iterator[index * _stride];
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::const_reference StlStridedIterator<IteratorType>::operator[](typename StlStridedIterator<IteratorType>::size_type index) const
    {
        return _iterator[index * _stride];
    }

    template <typename IteratorType>
    IteratorType StlStridedIterator<IteratorType>::GetBaseIterator() const
    {
        return _iterator;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::difference_type StlStridedIterator<IteratorType>::GetStride() const
    {
        return _stride;
    }

    //
    // Basic math operators defined as standalone functions
    //
    template <typename IteratorType>
    StlStridedIterator<IteratorType> operator+(const StlStridedIterator<IteratorType>& iterator, typename StlStridedIterator<IteratorType>::difference_type increment)
    {
        auto result = iterator;
        result += increment;
        return result;
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType> operator+(typename StlStridedIterator<IteratorType>::difference_type increment, const StlStridedIterator<IteratorType>& iterator)
    {
        auto result = iterator;
        result += increment;
        return result;
    }

    template <typename IteratorType>
    StlStridedIterator<IteratorType> operator-(const StlStridedIterator<IteratorType>& iterator, typename StlStridedIterator<IteratorType>::difference_type increment)
    {
        auto result = iterator;
        result -= increment;
        return result;
    }

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::difference_type operator-(const StlStridedIterator<IteratorType>& iterator1, const StlStridedIterator<IteratorType>& iterator2)
    {
        auto p1 = iterator1.GetBaseIterator();
        auto p2 = iterator2.GetBaseIterator();
        return (p1 - p2) / iterator1.GetStride();
    }

    // utility function for creating StlStridedIterator
    template <typename IteratorType>
    StlStridedIterator<IteratorType> MakeStlStridedIterator(const IteratorType& iterator, ptrdiff_t stride)
    {
        return { iterator, stride };
    }
}
}
