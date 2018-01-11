////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlStridedIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <iterator>

namespace ell
{
namespace utilities
{
    /// <summary> A simple stl-type iterator adapter that advances an iterator by a given stride. </summary>
    template <typename IteratorType>
    class StlStridedIterator : std::iterator<std::random_access_iterator_tag, typename std::iterator_traits<IteratorType>::value_type>
    {
    public:
        using Base = std::iterator<std::random_access_iterator_tag, typename std::iterator_traits<IteratorType>::value_type>;
        using value_type = typename std::iterator_traits<IteratorType>::value_type;
        using difference_type = typename Base::difference_type;
        using size_type = size_t;
        using reference = typename Base::reference;
        using const_reference = const reference;
        using pointer = typename Base::pointer;
        using const_pointer = const pointer;

        /// <summary> Constructor </summary>
        ///
        /// <param name="iterator"> The base iterator to use. </param>
        /// <param name="stride"> The offset between consecutive items. </param>
        StlStridedIterator(const IteratorType& iterator, difference_type stride);

        //
        // Required operations for random-access iterators:
        //

        bool operator==(const StlStridedIterator<IteratorType>& other) const;
        bool operator!=(const StlStridedIterator<IteratorType>& other) const;
        reference operator*();
        const_reference operator*() const;
        pointer operator->();
        const_pointer operator->() const;
        StlStridedIterator<IteratorType>& operator++();
        StlStridedIterator<IteratorType> operator++(int);
        StlStridedIterator<IteratorType>& operator--();
        StlStridedIterator<IteratorType> operator--(int);
        StlStridedIterator<IteratorType>& operator+=(difference_type increment);
        StlStridedIterator<IteratorType>& operator-=(difference_type increment);
        bool operator>(const StlStridedIterator<IteratorType>& other) const;
        bool operator<(const StlStridedIterator<IteratorType>& other) const;
        bool operator>=(const StlStridedIterator<IteratorType>& other) const;
        bool operator<=(const StlStridedIterator<IteratorType>& other) const;
        reference operator[](size_type index);
        const_reference operator[](size_type index) const;

        /// <summary> Gets the underlying iterator object. </summary>
        ///
        /// <returns> The underlying iterator object. </returns>
        IteratorType GetBaseIterator() const;

        /// <summary> Gets the stride between entries. </summary>
        ///
        /// <returns> The stride between entries. </returns>
        difference_type GetStride() const;

    protected:
        IteratorType _iterator;
        difference_type _stride;
    };

    // Basic math operators defined as standalone functions:
    template <typename IteratorType>
    StlStridedIterator<IteratorType> operator+(const StlStridedIterator<IteratorType>& iterator, typename StlStridedIterator<IteratorType>::difference_type increment);

    template <typename IteratorType>
    StlStridedIterator<IteratorType> operator+(typename StlStridedIterator<IteratorType>::difference_type increment, const StlStridedIterator<IteratorType>& iterator);

    template <typename IteratorType>
    StlStridedIterator<IteratorType> operator-(const StlStridedIterator<IteratorType>& iterator, typename StlStridedIterator<IteratorType>::difference_type increment);

    template <typename IteratorType>
    typename StlStridedIterator<IteratorType>::difference_type operator-(const StlStridedIterator<IteratorType>& iterator1, const StlStridedIterator<IteratorType>& iterator2);

    // utility function for creating StlStridedIterator
    template <typename IteratorType>
    StlStridedIterator<IteratorType> MakeStlStridedIterator(const IteratorType& iterator, ptrdiff_t stride);
}
}

#include "../tcc/StlStridedIterator.tcc"
