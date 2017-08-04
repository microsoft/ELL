////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "Matrix.h"
#include "Vector.h"

#ifdef USE_BLAS
#include "BlasWrapper.h"
#endif

// stl
#include <string>
#include <ostream>

namespace ell
{
namespace math
{
    /// \name Utility Functions
    /// @{

    /// <summary> Prints a matrix in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="M"> The matrix. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="indent"> (Optional) How many indent to print before the tensor. </param>
    /// <param name="maxRows"> (Optional) The maximum number of rows to print. </param>
    /// <param name="maxElementsPerRow"> (Optional) The maximum number of elements to print per row. </param>
    template <typename ElementType, MatrixLayout layout>
    void Print(ConstMatrixReference<ElementType, layout> M, std::ostream& stream, size_t indent = 0, size_t maxRows = std::numeric_limits<size_t>::max(), size_t maxElementsPerRow = std::numeric_limits<size_t>::max());

    /// <summary> Prints a matrix in initializer list format. </summary>
    ///
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="M"> The const matrix reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, MatrixLayout layout>
    std::ostream& operator<<(std::ostream& stream, ConstMatrixReference<ElementType, layout> M);

    /// @}

    /// \name Addition
    /// @{

    /// <summary> Adds a scalar to a matrix, matrix += scalar. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalar"> The scalar being added. </param>
    /// <param name="matrix"> The matrix to which the scalar is added. </param>
    template <typename ElementType, MatrixLayout layout>
    void Add(ElementType scalar, MatrixReference<ElementType, layout> matrix);

    /// <summary> Adds a scalar to a matrix, matrix += scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="matrix"> The matrix to which the scalar is added. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator+=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar);

    /// <summary> Subtracts a scalar from a matrix, matrix -= scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout type. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator-=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar);

    /// <summary> Generalized matrix matrix addition, matrixC = scalarA * matrixA + scalarB * matrixB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
    /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first matrix. </param>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="scalarB"> The scalar that multiplies the second matrix. </param>
    /// <param name="matrixB"> The second matrix. </param>
    /// <param name="matrixC"> A matrix used to store the result. </param>
    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void Add(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC);

    /// <summary> Sums each column of a matrix and stores the results in a row vector. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="vector"> The vector used to store the result. </param>
    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseSum(ConstMatrixReference<ElementType, layout> matrix, VectorReference<ElementType, VectorOrientation::row> vector);

    /// @}

    /// \name Multiplication
    /// @{

    /// <summary> Multiplies a matrix by a scalar, matrix *= scalar. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalar"> The scalar that multiplies the matrix. </param>
    /// <param name="matrix"> The matrix which is multiplied by scalar. </param>
    template <typename ElementType, MatrixLayout layout>
    void Multiply(ElementType scalar, MatrixReference<ElementType, layout> matrix);

    /// <summary> Multiplies a matrix by a scalar, matrix *= scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout type. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator*=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar);

    /// <summary> Divides a matrix by a scalar, matrix /= scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout type. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator/=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar);

    /// <summary> Generalized matrix vector multiplication, vectorB = scalarA * matrix * vectorA + scalarB * vectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the matrix. </param>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="vectorA"> The column vector that multiplies the matrix from the right. </param>
    /// <param name="scalarB"> The scalar that multiplies vectorB. </param>
    /// <param name="vectorB"> A column vector, multiplied by scalarB and used to store the result. </param>
    template <typename ElementType, MatrixLayout layout>
    void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ElementType scalarB, VectorReference<ElementType, VectorOrientation::column> vectorB);

    /// <summary> Generalized (left-side) matrix row-vector multiplication, vectorB = scalarA * vectorA * matrix + scalarB * vectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the matrix. </param>
    /// <param name="vectorA"> The row vector that multiplies the matrix from the left. </param>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalarB"> The scalar that multiplies vectorB. </param>
    /// <param name="vectorB"> A row vector, multiplied by scalarB and used to store the result. </param>
    template <typename ElementType, MatrixLayout layout>
    void Multiply(ElementType scalarA, ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, VectorReference<ElementType, VectorOrientation::row> vectorB);

    /// <summary> Generalized matrix matrix multiplication, matrixC = scalarA * matrixA * matrixB + scalarC * matrixC. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
    /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first matrix. </param>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="matrixB"> The second matrix. </param>
    /// <param name="scalarC"> The scalar that multiplies the third matrix. </param>
    /// <param name="matrixC"> A third matrix, multiplied by scalarC and used to store the result. </param>
    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC);

    /// <summary> Multiplies a matrix by scalarA and then adds scalarB to it, matrix = scalarA * matrix + scalarB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the matrix. </param>
    /// <param name="scalarB"> The scalar added to the matrix. </param>
    /// <param name="matrix"> The matrix being modified. </param>
    template <typename ElementType, MatrixLayout layout>
    void MultiplyAdd(ElementType scalarA, ElementType scalarB, MatrixReference<ElementType, layout> matrix);

    /// <summary> Matrix matrix element wise multiplication, matrixC = matrixA .* matrixB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
    /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="matrixB"> The second matrix. </param>
    /// <param name="matrixC"> A matrix used to store the result in the layout of first matrix. </param>
    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ElementwiseMultiply(ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC);

    /// @}

    namespace Internal
    {
        template <ImplementationType type>
        struct MatrixOperations
        {};

        template <>
        struct MatrixOperations<ImplementationType::native>
        {
            static std::string GetImplementationName() { return "Native"; }

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
            static void Add(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC);

            template <typename ElementType, MatrixLayout layout>
            static void Multiply(ElementType scalar, MatrixReference<ElementType, layout> matrix);

            template <typename ElementType, MatrixLayout layout>
            static void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ElementType scalarB, VectorReference<ElementType, VectorOrientation::column> vectorB);

            template <typename ElementType, MatrixLayout layout>
            static void Multiply(ElementType scalarA, ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, VectorReference<ElementType, VectorOrientation::row> vectorB);

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
            static void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC);
        };

#ifdef USE_BLAS
        template <>
        struct MatrixOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBlas"; }

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
            static void Add(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC);

            template <typename ElementType, MatrixLayout layout>
            static void Multiply(ElementType scalar, MatrixReference<ElementType, layout> matrix);
            
            template <typename ElementType, MatrixLayout layout>
            static void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ElementType scalarB, VectorReference<ElementType, VectorOrientation::column> vectorB);

            template <typename ElementType, MatrixLayout layout>
            static void Multiply(ElementType scalarA, ConstVectorReference<ElementType, VectorOrientation::row> vectorA, const ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, VectorReference<ElementType, VectorOrientation::row> vectorB);

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
            static void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC);
        };

#else
        template <>
        struct MatrixOperations<ImplementationType::openBlas> : public MatrixOperations<ImplementationType::native>
        {
        };

#endif // USE_BLAS
    }
}
}

#include "../tcc/MatrixOperations.tcc"
