////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IDataVector.h (dataset) // TODO rename
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
    /// <summary> Interface for infinite-dimensional vectors of double numbers. Each implementation of
    /// this interface has a mathematical dimension of infinity and is made up of an explicitly specified
    /// prefix followed by an implicit suffix of zeros. </summary>
    class IDataVector 
    {
    public:
        /// <summary> Adds a value at the end of the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(size_t index, double value = 1.0) = 0;

        /// <summary> Returns the Size of the vector. </summary>
        ///
        /// <returns> The size of the vector. </returns>
        virtual size_t Size() const = 0; // TODO change this to GetFirstSuffixIndex()

        /// <summary> Computes the squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm. </returns>
        virtual double Norm2() const = 0;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="p_other"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const double* p_other) const = 0;

        /// <summary>
        /// Performs the operation: (*p_other) += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="p_other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const = 0;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const = 0;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const = 0;
    };

    template<class DerivedType>
    class DataVectorBase : public IDataVector
    {
    public:
        /// <summary> Computes the squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm. </returns>
        virtual double Norm2() const override;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="p_other"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const double* p_other) const override;

        /// <summary>
        /// Performs the operation: (*p_other) += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="p_other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override;

        /// <summary> Human readable printout to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;  
    };
}
}

#include "../tcc/DataVector.tcc"