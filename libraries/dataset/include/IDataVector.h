// IDataVector.h

#pragma once

#include "IVector.h"

// types
#include "types.h"

// stl
#include <vector>
#include <functional>

namespace dataset
{
    /// Base class for infinite-dimensional vectors of double numbers. Each implementation of this class has a mathematical dimension of
    /// infinity and includes an explicitly specified prefix followed by an implicit suffix of zeros. 
    class IDataVector : public linear::IVector
    {
    public:

        enum class type {dense_double, dense_float, sparse_double, sparse_float, sparse_short, sparse_binary, uncompressed_sparse_binary, zero, ones, segmented};

        /// \returns The type of the std::vector
        ///
        virtual type GetType() const = 0;

        /// Adds a value at the end of the std::vector
        ///
        virtual void PushBack(uint64 index, double value = 1.0) = 0;

        /// Deletes all of the std::vector content and sets its Size to zero, but does not deallocate its memory
        ///
        virtual void Reset() = 0;

        /// \returns The number of non-zeros
        ///
        virtual uint64 NumNonzeros() const =0;
    };
}
