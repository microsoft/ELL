////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Scalar.h"

#include <utilities/include/MemoryLayout.h>

#include <functional>

namespace ell
{
namespace value
{

    class Matrix;
    class Vector;

    Scalar Sum(Matrix matrix, Scalar initialValue);

    /// <summary> Creates a for loop over the matrix </summary>
    /// <param name="matrix"> The instance of Matrix that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Matrix matrix, std::function<void(Scalar, Scalar)> fn);

    Matrix GEMM(Matrix m1, Matrix m2);

    Vector GEMV(Matrix m, Vector v);

    Matrix operator+(Matrix, Matrix);
    Matrix operator+(Matrix, Scalar);

    Matrix operator-(Matrix, Matrix);
    Matrix operator-(Matrix, Scalar);

    Matrix operator*(Matrix, Scalar);

    Matrix operator/(Matrix, Scalar);

} // namespace value
} // namespace ell
