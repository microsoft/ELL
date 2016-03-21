////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DoubleVector.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IVector.h"

// types
#include "types.h"
#include "StlIndexValueIterator.h"

// stl
#include <iostream>
#include <type_traits>

namespace linear
{
    /// <summary> A double vector. </summary>
    class DoubleVector : public IVector
    {
    public:
        using Iterator = types::VectorIndexValueIterator<double>;

        /// <summary> Constructs an instance of DoubleVector. </summary>
        ///
        /// <param name="size"> The size. </param>
        DoubleVector(uint64 size = 0);

        /// <summary> Copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        DoubleVector(const DoubleVector&) = default;

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="parameter1"> [in,out] The first parameter. </param>
        DoubleVector(DoubleVector&&) = default;

        /// <summary> Constructor to copy from a std::vector<double>
        ///
        /// <param name="v"> The vector to copy from </param>
        DoubleVector(const std::vector<double>& v);

        /// <summary> Constructor to move from a std::vector<double>
        ///
        /// <param name="v"> The vector to move from </param>
        DoubleVector(std::vector<double>&& v);

        /// <summary> Converting constructor. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <typeparam name="IIndexValueIterator"> Type of the index value iterator. </typeparam>
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="indexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        DoubleVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Type-conversion operator into a std::vector<double>
        operator std::vector<double>() const &;

        /// <summary> Type-conversion operator into a std::vector<double>, allowing non-const ref
        operator std::vector<double> & () &;

        /// <summary> Type-conversion operator into a const std::vector<double> reference
        operator std::vector<double> const & () const &;

        /// <summary> Type-conversion operator into a std::vector<double>, allowing move semantics
        operator std::vector<double> && () &&;

        /// <summary>
        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its
        /// memory.
        /// </summary>
        void Reset();

        /// <summary> Returns a reference to an element in the vector. </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The element at position index. </returns>
        double& operator[](uint64 index);

        /// <summary> Returns a const reference to an element in the vector. </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The element at position index. </returns>
        double operator[](uint64 index) const;

        /// <summary> Returns a pointer to the beginning of the vector</summary>
        ///
        /// <returns> A pointer to the first element in the vector. </returns>
        double* GetDataPointer();

        /// <summary> Returns a const pointer to the beginning of the vector</summary>
        ///
        /// <returns> A pointer to the first element in the vector. </returns>
        const double* GetDataPointer() const;

        /// <summary> Returns an IndexValueIterator for the vector. </summary>
        ///
        /// <returns> An IndexValueIterator. </returns>
        Iterator GetIterator() const;

        /// <summary> Returns the Size of the vector. </summary>
        ///
        /// <returns> The size of the vector. </returns>
        virtual uint64 Size() const override;

        /// <summary> Computes the squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm. </returns>
        virtual double Norm2() const override;

        /// <summary> Performs the operation: other += scalar * (*this), where other is a std::vector. </summary>
        ///
        /// <param name="other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;
        using IVector::AddTo;

        /// <summary> Scales the vector by a scalar. </summary>
        ///
        /// <param name="s"> The scalar. </param>
        void Scale(double s);

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> The dot product. </returns>
        virtual double Dot(const double* p_other) const override;

        /// <summary> Prints the vector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

    private:
        std::vector<double> _data;
    };
}

#include "../tcc/DoubleVector.tcc"

