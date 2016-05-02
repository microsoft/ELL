////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     BiMatrix.h (linear)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <vector>

namespace linear
{
    /// <summary>
    /// A matrix type that stores two identical copies of the entries, one in row major orientation
    /// and one in column major.
    /// </summary>
    ///
    /// <typeparam name="RowMatrixType"> The type used to store each copy of the matrix. </typeparam>
    template<typename RowMatrixType>
    class BiMatrix : public RowMatrixType
    {
    public:
        
        /// <summary> The type of each row. </summary>
        using RowType = typename RowMatrixType::RowType;

        /// <summary> Constructs a bioriented matrix. </summary>
        BiMatrix() = default;

        /// <summary> Move constructor. </summary>
        BiMatrix(BiMatrix<RowMatrixType>&&) = default;

        /// <summary> Deleted copy constructor. </summary>
        BiMatrix(const BiMatrix<RowMatrixType>&) = delete;

        /// <summary> Gets a reference to a column. </summary>
        ///
        /// <param name="index"> Zero-based index of the column. </param>
        ///
        /// <returns> A reference to the column. </returns>
        const RowType& GetColumn(uint64_t index) const;

        /// <summary> Performs a general vector-matrix product: y = alpha * x * M + beta * y. </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] The y vector. </param>
        /// <param name="alpha"> The alpha parameter. </param>
        /// <param name="beta"> The beta parameter. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

    private:
        RowMatrixType _transp;
    };
}

#include "../tcc/BiMatrix.tcc"
