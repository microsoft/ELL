////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IMatrix.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Base.h"

// types
#include "types.h"

// stl
#include <vector>

namespace linear
{
    /// <summary> Base class for matrices of double numbers. </summary>
    class IMatrix : public Base
    {
    public:

        /// <summary> Returns the number of rows in the matrix. </summary>
        ///
        /// <returns> The total number of rows. </returns>
        virtual uint64 NumRows() const = 0;

        /// <summary> Returns the number of columns in the matrix. </summary>
        ///
        /// <returns> The total number of columns. </returns>
        virtual uint64 NumColumns() const = 0;

        /// <summary> Performs a general matrix-vector product: y = alpha * M * x + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        void Gemv(const std::vector<double>& x, std::vector<double>& y, double alpha = 1.0, double beta = 0.0) const;

        /// <summary> Performs a general matrix-vector product: y = alpha * M * x + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const =0;

        /// <summary> Performs a general vector-matrix product: y = alpha * x * M + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        void Gevm(const std::vector<double>& x, std::vector<double>& y, double alpha = 1.0, double beta = 0.0) const;

        /// <summary> Performs a general vector-matrix product: y = alpha * x * M + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const =0;
    };
}
