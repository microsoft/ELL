////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
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

        /// Proceeds to the Next row
        ///
        virtual void Next() = 0;

        /// Returns the weight of the current example
        ///
        virtual T Get() = 0;
    };
}
