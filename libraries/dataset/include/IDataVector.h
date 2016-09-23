////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace emll
{
namespace dataset
{
    /// <summary> Base class for infinite-dimensional vectors of double numbers. Each implementation of
    /// this class has a mathematical dimension of infinity and includes an explicitly specified
    /// prefix followed by an implicit suffix of zeros. </summary>
    class IDataVector 
    {
    public:
        /// <summary> Adds a value at the end of the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(uint64_t index, double value = 1.0) = 0;

        /// <summary> Returns the number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64_t NumNonzeros() const = 0;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const = 0;

        /// <summary> Returns the Size of the vector. </summary>
        ///
        /// <returns> The size of the vector. </returns>
        virtual uint64_t Size() const = 0;

        /// <summary> Computes the squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm. </returns>
        virtual double Norm2() const = 0;

        /// <summary>
        /// Performs the operation: (*p_other) += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="p_other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const = 0;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="p_other"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const double* p_other) const = 0;


        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const = 0;
    };
}
}
