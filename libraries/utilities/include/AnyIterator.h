////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AnyIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IIterator.h"

// stl
#include <functional>
#include <memory>
#include <utility>

namespace ell
{
/// <summary> utilities namespace </summary>
namespace utilities
{
    // Forward declaration necessary so we can declare the MakeAnyIterator functions, so we can then declare them as friends.
    template <typename ValueType>
    class AnyIterator;

#ifndef SWIG
    template <typename IteratorType, typename ValueType = typename std::decay<decltype(std::declval<IteratorType>().Get())>::type>
    AnyIterator<ValueType> MakeAnyIterator(IteratorType&& inIterator);
#endif

    /// <summary>
    /// A type-erased wrapper for an iterator over a collection of objects of type T.
    /// Note: the original iterator must be movable or copyable.
    ///</summary>
    ///
    /// <typeparam name="ValueType"> Type of the elements being iterated over. </typeparam>
    template <typename ValueType>
    class AnyIterator
    {
    public:
        /// <summary> default Constructor</summary>
        AnyIterator() = default;

        /// <summary> Constructor that takes a shared pointer to an IIterator (used by MakeAnyIterator helper function) </summary>
        AnyIterator(std::shared_ptr<IIterator<ValueType>> iterator);

        /// <summary> Default destructor </summary>
        ~AnyIterator() = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if valid, false if not. </returns>
        bool IsValid() const;

        /// <summary> Returns true if the iterator knows its size. </summary>
        ///
        /// <returns> true if NumItemsLeft returns a valid number, false if not. </returns>
        bool HasSize() const;

        /// <summary>
        /// Returns the number of iterates left in this iterator, including the current one.
        /// </summary>
        ///
        /// <returns> The total number of iterates left. </returns>
        size_t NumItemsLeft() const;

        /// <summary> Proceeds to the Next item. </summary>
        void Next();

        /// <summary> Returns a const reference to the current item`. </summary>
        ///
        /// <returns> A const reference to the current item; </returns>
        ValueType Get() const;

    private:
        std::shared_ptr<IIterator<ValueType>> _iterator = nullptr;
    };
}
}

#include "../tcc/AnyIterator.tcc"
