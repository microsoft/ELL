////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ZipIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <iterator>
#include <tuple>

namespace ell
{
namespace utilities
{
    /// <summary>
    ///   A simple stl-type iterator adapter that joins a set of iterators into a single input iterator. When
    ///   the new iterator is dereferenced, it returns a tuple containing the elements pointed to by each of
    ///   the underlying iterators. When the new iterator is advanced, each of the underlying iterators is advanced.
    /// </summary>
    template <typename... IteratorTypes>
    class ZipIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::tuple<typename std::iterator_traits<IteratorTypes>::value_type...>;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using reference = value_type;
        using pointer = value_type*;
        using const_reference = const reference;
        using const_pointer = const pointer;

        /// <summary> Constructor </summary>
        ///
        /// <param name="iterators"> The base iterators to zip together. </param>
        ZipIterator(IteratorTypes... iterators);

        //
        // Required operations for input iterators:
        //
        bool operator==(const ZipIterator<IteratorTypes...>& other) const;
        bool operator!=(const ZipIterator<IteratorTypes...>& other) const;
        reference operator*()
        {
            return std::apply([](auto&... x) { return std::make_tuple((*x)...); }, _iterators);
        }

        const_reference operator*() const
        {
            return std::apply([](const auto&... x) { return std::make_tuple((*x)...); }, _iterators);
        }

        pointer operator->();
        const_pointer operator->() const;

        /// <summary> Advance the iterator and return the new position (preincrement) </summary>
        /// When the end of any of the inputs is encountered, returns `end()`.
        ZipIterator<IteratorTypes...>& operator++();

        /// <summary> Advance the iterator and return the old position (postincrement) </summary>
        /// When the end of any of the inputs is encountered, returns `end()`.
        ZipIterator<IteratorTypes...> operator++(int);

    private:
        ZipIterator(const std::tuple<IteratorTypes...>& iterators) :
            _iterators(iterators) {}
        std::tuple<IteratorTypes...> _iterators;
    };

    /// A struct containing a begin and end iterator
    template <typename... IteratorTypes>
    struct ZipRange
    {
        ZipIterator<IteratorTypes...> beginIter;
        ZipIterator<IteratorTypes...> endIter;
    };

    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...> begin(const ZipRange<IteratorTypes...>& range)
    {
        return range.beginIter;
    }

    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...> end(const ZipRange<IteratorTypes...>& range)
    {
        return range.endIter;
    }

    // utility function for creating a ZipIterator
    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...> MakeZipIterator(const IteratorTypes&... iterators);

    // utility function for creating a ZipRange
    template <typename... ContainerTypes>
    ZipRange<typename ContainerTypes::const_iterator...> MakeZipRange(const ContainerTypes&... iterators);
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    namespace detail
    {
        template <typename... IteratorTypes, size_t... Index>
        bool AnyEqual(const std::tuple<IteratorTypes...>& a, const std::tuple<IteratorTypes...>& b, std::index_sequence<Index...>)
        {
            return ((std::get<Index>(a) == std::get<Index>(b)) || ...);
        }
    } // namespace

    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...>::ZipIterator(IteratorTypes... iterators) :
        _iterators(iterators...)
    {
    }

    //
    // Required operations for random-access iterators:
    //

    template <typename... IteratorTypes>
    bool ZipIterator<IteratorTypes...>::operator==(const ZipIterator<IteratorTypes...>& other) const
    {
        // We return 'true' if any of our component iterators equal the corresponding query iterator.
        // Typically we use operator== to check against the end of the sequence, and we want to stop
        // when we encounter the first "end" iterator.
        return detail::AnyEqual(_iterators, other._iterators, std::index_sequence_for<IteratorTypes...>{});
    }

    template <typename... IteratorTypes>
    bool ZipIterator<IteratorTypes...>::operator!=(const ZipIterator<IteratorTypes...>& other) const
    {
        return !((*this) == other);
    }

    template <typename... IteratorTypes>
    typename ZipIterator<IteratorTypes...>::pointer ZipIterator<IteratorTypes...>::operator->()
    {
        return _iterators;
    }

    template <typename... IteratorTypes>
    typename ZipIterator<IteratorTypes...>::const_pointer ZipIterator<IteratorTypes...>::operator->() const
    {
        return _iterators;
    }

    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...>& ZipIterator<IteratorTypes...>::operator++()
    {
        std::apply([](auto&... x) { return std::make_tuple((++x)...); }, _iterators);
        return *this;
    }

    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...> ZipIterator<IteratorTypes...>::operator++(int)
    {
        auto temp = _iterators;
        std::apply([](auto&... x) { return std::make_tuple((++x)...); }, _iterators);
        return { temp };
    }

    // utility function for creating ZipIterator
    template <typename... IteratorTypes>
    ZipIterator<IteratorTypes...> MakeZipIterator(const IteratorTypes&... iterators)
    {
        return ZipIterator<IteratorTypes...>(iterators...);
    }

    template <typename... ContainerTypes>
    ZipRange<typename ContainerTypes::const_iterator...> MakeZipRange(const ContainerTypes&... containers)
    {
        return { MakeZipIterator(std::cbegin(containers)...), MakeZipIterator(std::cend(containers)...) };
    }

} // namespace utilities
} // namespace ell

#pragma endregion implementation
