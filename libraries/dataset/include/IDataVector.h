////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     IDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IVector.h"

// stl
#include <cstdint>
#include <vector>
#include <functional>
#include <memory>

namespace dataset
{
    /// <summary> Base class for infinite-dimensional vectors of double numbers. Each implementation of
    /// this class has a mathematical dimension of infinity and includes an explicitly specified
    /// prefix followed by an implicit suffix of zeros. </summary>
    class IDataVector : public linear::IVector
    {
    public:

        enum class type {none, dense_double, dense_float, sparse_double, sparse_float, sparse_short, sparse_binary, uncompressed_sparse_binary, zero, ones, segmented};

        /// <summary> \returns The type of the std::vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const = 0;

        /// <summary> Adds a value at the end of the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(uint64_t index, double value = 1.0) = 0;

        /// <summary> Deletes all of the std::vector content and sets its Size to zero, but does not
        /// deallocate its memory. </summary>
        virtual void Reset() = 0;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64_t NumNonzeros() const = 0;
        
        /// <summary> Makes a deep copy of the datavector </summary>
        ///
        /// <returns> A deep copy of the datavector </summary>
        virtual std::unique_ptr<IDataVector> Clone() const = 0;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray(uint64_t size) const = 0;
    };
}
