////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     IIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

/// stl
#include <vector>

namespace utilities
{
    /// <summary> Abstract base class for an iterator over a collection of objects of type T. </summary>
    ///
    /// <typeparam name="T"> Type of the elements being iterated over. </typeparam>
    template <typename T>
    class IIterator
    {
    public:

        /// default virtual destructor
        ///
        virtual ~IIterator() = default;

        /// Returns true if the iterator is currently pointing to a valid iterate
        ///
        virtual bool IsValid() const = 0;

        /// Proceeds to the Next item
        ///
        virtual void Next() = 0;

        /// Returns the current item
        ///
        virtual T Get() const = 0;
    };
}
