////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>

namespace ell
{
namespace utilities
{
    /// <summary> Abstract base class for an iterator over a collection of objects of type T. </summary>
    ///
    /// <typeparam name="ValueType"> Type of the elements being iterated over. </typeparam>
    template <typename ValueType>
    class IIterator
    {
    public:
        virtual ~IIterator() = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if valid, false if not. </returns>
        virtual bool IsValid() const = 0;

        /// <summary> Returns true if the iterator knows its size. </summary>
        ///
        /// <returns> true if NumItemsLeft returns a valid number, false if not. </returns>
        virtual bool HasSize() const { return false; }

        /// <summary>
        /// Returns the number of iterates left in this iterator, including the current one.
        /// </summary>
        ///
        /// <returns> The total number of iterates left. </returns>
        virtual size_t NumItemsLeft() const { return 0; }

        /// <summary> Proceeds to the Next item. </summary>
        virtual void Next() = 0;

        /// <summary> Returns a const reference to the current item. </summary>
        ///
        /// <returns> A const reference to the current item; </returns>
        virtual ValueType Get() const = 0;
    };
}
}
