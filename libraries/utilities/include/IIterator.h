// IIterator.h

#pragma once

/// stl
#include <vector>
using std::vector;

namespace utilities
{
    template <typename T>
    class IIterator
    {
    public:
        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        virtual bool IsValid() const = 0;

        /// Proceeds to the Next row
        ///
        virtual void Next() = 0;

        /// \returns The weight of the current example
        ///
        virtual T Get() = 0;
    };
}
