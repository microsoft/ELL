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
#include "RealArray.h"

// stl
#include <iostream>
#include <type_traits>

namespace linear
{
    /// <summary> A double vector. </summary>
    class DoubleVector : public IVector
    {
    public:

        // ####
        // copy / move from std::vector
        DoubleVector(const std::vector<double>& v) : _data(v) {};

        DoubleVector(std::vector<double>&& v) : _data(std::forward<std::vector<double>>(v)) {};

        // op std::vector
        operator std::vector<double>() const &
        {
            return _data;
        } // const ref

        operator std::vector<double> & () &
        {
            return std::ref(_data);
        }; // ref

        operator std::vector<double> && () &&
        {
            return std::move(_data);
        }; // move
        // ####



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

        /// <summary> Converting constructor. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <typeparam name="IIndexValueIterator"> Type of the index value iterator. </typeparam>
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="indexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        DoubleVector(IndexValueIteratorType indexValueIterator);

        /// <summary>
        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its
        /// memory.
        /// </summary>
        void Reset();

        /// #### TODO: document
        double& operator[](size_t index) { return _data[index]; }
        double operator[](size_t index) const { return _data[index]; }
        double* GetDataPointer() { return _data.data(); }
        const double* GetDataPointer() const { return _data.data(); }
        types::SparseStlIterator<double> GetIterator() const { return types::GetIterator(_data); }

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

