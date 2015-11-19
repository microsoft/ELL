// IDataVector.h

#pragma once

#include "types.h"


#include "IVector.h"
using linear::IVector;

// stl
#include <vector>
using std::vector;

#include <functional>
using std::function;

namespace dataset
{
    /// Base class for infinite-dimensional vectors of double numbers. Each implementation of this class has a mathematical dimension of
    /// infinity and includes an explicitly specified prefix followed by an implicit suffix of zeros. 
    class IDataVector : public IVector
    {
    public:

        enum class type {dense_double, dense_float, sparse_double, sparse_float, sparse_short, sparse_binary, uncompressed_sparse_binary, zero, ones, segmented};

        /// \returns The type of the vector
        ///
        virtual type GetType() const = 0;

        /// Adds a value at the end of the vector
        ///
        virtual void PushBack(uint64 index, double value = 1.0) = 0;

        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its memory
        ///
        virtual void Reset() = 0;

        /// Calls a callback function for each non-zero entry in the vector, in order of increasing index
        ///
        //virtual void foreach_nonzero(function<void(uint64, double)> func, uint64 index_offset = 0) const = 0; // TODO

        /// \returns The number of non-zeros
        ///
        virtual uint64 NumNonzeros() const =0;
    };
}