////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IVector.h"

// types
#include "types.h"

// stl
#include <vector>
#include <functional>

namespace dataset
{
    /// <summary> Base class for infinite-dimensional vectors of double numbers. Each implementation of
    /// this class has a mathematical dimension of infinity and includes an explicitly specified
    /// prefix followed by an implicit suffix of zeros. </summary>
    class IDataVector : public linear::IVector
    {
    public:

        enum class type {dense_double, dense_float, sparse_double, sparse_float, sparse_short, sparse_binary, uncompressed_sparse_binary, zero, ones, segmented};

        /// <summary> \returns The type of the std::vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const = 0;

        /// <summary> Adds a value at the end of the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void PushBack(uint64 index, double value = 1.0) = 0;

        /// <summary> Deletes all of the std::vector content and sets its Size to zero, but does not
        /// deallocate its memory. </summary>
        virtual void Reset() = 0;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64 NumNonzeros() const =0;
    };
}
