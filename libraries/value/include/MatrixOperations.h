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

    /// <summary> Reinterprets the given data value as a matrix of the given size </summary>
    Matrix ToMatrix(Value data, int numRows, int numCols);

    Scalar Sum(Matrix matrix, Scalar initialValue);

    /// <summary> Creates a for loop over the matrix </summary>
    /// <param name="matrix"> The instance of Matrix that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Matrix matrix, std::function<void(Scalar, Scalar)> fn);

    /// <summary> Creates a for loop over the matrix </summary>
    /// <param name="name"> A name that can be used by the emitter context to tag this loop in the emitted code </param>
    /// <param name="matrix"> The instance of Matrix that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(const std::string& name, Matrix matrix, std::function<void(Scalar, Scalar)> fn);

    Matrix GEMM(Matrix m1, Matrix m2);

    Vector GEMV(Matrix m, Vector v);

} // namespace value
} // namespace ell
