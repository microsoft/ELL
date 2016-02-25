////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     BiMatrix.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace linear
{
    // A matrix type that stores two identical copies of the entries, one in row major orientation and one in column major
    //
    template<typename RowMatrixType>
    class BiMatrix : public RowMatrixType
    {
    public:
        
        /// <summary> The type of each row. </summary>
        using RowType = typename RowMatrixType::RowType;

        /// <summary> Constructs a bioriented matrix. </summary>
        BiMatrix() = default;

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="parameter1"> [in,out] The first parameter. </param>
        BiMatrix(BiMatrix<RowMatrixType>&&) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        BiMatrix(const BiMatrix<RowMatrixType>&) = delete;

        /// <summary> Gets a reference to a column. </summary>
        ///
        /// <param name="index"> Zero-based index of the column. </param>
        ///
        /// <returns> A reference to the column. </returns>
        const RowType& GetColumn(uint64 index) const;

        /// <summary>
        /// Performs a general vector-matrix product: y = alpha * x * M + beta * y.
        /// </summary>
        ///
        /// <param name="p_x"> The x vector. </param>
        /// <param name="p_y"> [in,out] the y vector. </param>
        /// <param name="alpha"> The alpha. </param>
        /// <param name="beta"> The beta. </param>
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

    private:
        RowMatrixType _transp;
    };
}

#include "../tcc/BiMatrix.tcc"
