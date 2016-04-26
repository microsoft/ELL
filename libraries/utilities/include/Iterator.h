////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Iterator.h (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

/// stl
#include <functional>
#include <memory>
#include <utility>

namespace utilities
{
    // Forward declaration necessary so we can declare the MakeIterator functions, so we can then declare them as friends.
    template <typename ValueType> class Iterator;

#ifndef SWIG
    template <typename IteratorType, typename ValueType = typename std::decay<decltype(std::declval<IteratorType>().Get())>::type>
    auto MakeIterator(IteratorType inIterator) ->Iterator<ValueType>;
#endif

    /// <summary> 
    /// A type-erased wrapper for an iterator over a collection of objects of type ValueType. 
    /// Note: the original iterator must be movable or copyable.
    ///</summary>    
    ///
    /// <typeparam name="ValueType"> Type of the elements being iterated over. </typeparam>
    template <typename ValueType>
    class Iterator
    {
    public:

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid();

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The value of the current iterate. </returns>
        ValueType Get();

        // friend declarations of the MakeIterator factory functions
        template <typename IteratorType, typename ValueType2>
        friend auto MakeIterator(IteratorType iter) -> Iterator<ValueType2>;
    private:

        Iterator(std::shared_ptr<void> mem, std::function<bool()> isValid, std::function<void()> next, std::function<ValueType()> get);
        std::function<bool()> _isValid;
        std::function<void()> _next;
        std::function<ValueType()> _get;
        std::shared_ptr<void> _originalIterator;
    };
}

#include "../tcc/Iterator.tcc"
