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

    /// <summary> Adds a scalar to a matrix, matrix += scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="matrix"> The matrix to which the scalar is added. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator+=(MatrixReference<MatrixElementType, layout> matrix, ScalarType scalar);

    /// <summary> Adds a matrix to a matrix, matrixB += matrixA. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> MatrixA layout. </typeparam>
    /// <typeparam name="layoutB"> MatrixB layout. </typeparam>
    /// <param name="matrixB"> The left-hand side matrix, the one being modified. </param>
    /// <param name="matrixA"> The right-hand side matrix. </param>
    template <typename MatrixElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void operator+=(MatrixReference<MatrixElementType, layoutB> matrixB, ConstMatrixReference<MatrixElementType, layoutA> matrixA);

    /// <summary> Subtracts a scalar from a matrix, matrix -= scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout type. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator-=(MatrixReference<MatrixElementType, layout> matrix, ScalarType scalar);

    /// <summary> Subtracts a matrix from another matrix, matrixB -= matrixA. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> MatrixA layout. </typeparam>
    /// <typeparam name="layoutB"> MatrixB layout. </typeparam>
    /// <param name="matrixB"> The left-hand side matrix, the one being modified. </param>
    /// <param name="matrixA"> The right-hand side matrix. </param>
    template <typename MatrixElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void operator-=(MatrixReference<MatrixElementType, layoutB> matrixB, ConstMatrixReference<MatrixElementType, layoutA> matrixA);

    /// <summary> Multiplies a matrix by a scalar, matrix *= scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout type. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator*=(MatrixReference<MatrixElementType, layout> matrix, ScalarType scalar);

    /// <summary> Divides a matrix by a scalar, matrix /= scalar. </summary>
    ///
    /// <typeparam name="MatrixElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout type. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename MatrixElementType, MatrixLayout layout, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator/=(MatrixReference<MatrixElementType, layout> matrix, ScalarType scalar);

    /// <summary> Adds a scalar to a matrix in place, matrix += scalar. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalar"> The scalar being added. </param>
    /// <param name="matrix"> The matrix to which the scalar is added. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void AddUpdate(ElementType scalar, MatrixReference<ElementType, layout> matrix);

    /// <summary> Adds a matrix to another matrix in place, matrixB += matrixA. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> MatrixA layout. </typeparam>
    /// <typeparam name="layoutB"> MatrixB layout. </typeparam>
    /// <param name="matrixA"> The matrix being added to matrixB. </param>
    /// <param name="matrixB"> The matrix to which matrixA is added. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void AddUpdate(ConstMatrixReference<ElementType, layoutA> matrixA, MatrixReference<ElementType, layoutB> matrixB);

    /// <summary> Adds a scalar to a matrix and stores the result in another matrix, output = scalar + matrix. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <typeparam name="outputLayout"> Output matrix layout. </typeparam>
    /// <param name="scalar"> The scalar being added. </param>
    /// <param name="matrix"> The matrix to which the scalar is added. </param>
    /// <param name="output"> The matrix where the result is stored. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout, MatrixLayout outputLayout>
    void AddSet(ElementType scalar, ConstMatrixReference<ElementType, layout> matrix, MatrixReference<ElementType, outputLayout> output);

    /// <summary> Adds a matrix to another matrix and stores the result in a third matrix, output = matrixB + matrixA. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> MatrixA layout. </typeparam>
    /// <typeparam name="layoutB"> MatrixB layout. </typeparam>
    /// <typeparam name="outputLayout"> Output matrix layout. </typeparam>
    /// <param name="matrixA"> The first matrix being added. </param>
    /// <param name="matrixB"> The second matrix being added. </param>
    /// <param name="output"> The matrix used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void AddSet(ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output);

    /// <summary> Multiplies a matrix by a scalar in place, matrix *= scalar. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalar"> The scalar that multiplies the matrix. </param>
    /// <param name="matrix"> The matrix which is multiplied by the scalar. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void ScaleUpdate(ElementType scalar, MatrixReference<ElementType, layout> matrix);

    /// <summary> Multiplies a matrix by a scalar and stores the result in an output matrix, output = scalar * matrix. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Input matrix layout. </typeparam>
    /// <typeparam name="outputLayout"> Output matrix layout. </typeparam>
    /// <param name="scalar"> The scalar that multiplies the matrix. </param>
    /// <param name="matrix"> The matrix which is multiplied by the scalar. </param>
    /// <param name="output"> The matrix used to store the output. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout matrixLayout, MatrixLayout outputLayout>
    void ScaleSet(ElementType scalar, ConstMatrixReference<ElementType, matrixLayout> matrix, MatrixReference<ElementType, outputLayout> output);

    /// <summary> A stub class that represents the all-ones matrix. </summary>
    struct OnesMatrix {};

    /// @{
    /// <summary>
    /// Performs the linear operation matrixB = scalarA * matrixA + scalarB * matrixB.
    /// </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="layoutA"> MatrixA layout. </typeparam>
    /// <typeparam name="layoutB"> MatrixB layout. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first matrix: either a real number or One().</param>
    /// <param name="matrixA"> The first matrix or OnesMatrix(). </param>
    /// <param name="scalarB"> The scalar that multiplies the second matrix: either a real number or One() </param>
    /// <param name="matrixB"> The second matrix, which is updated. </param>

    // matrixB += scalarA * matrixA
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, One, MatrixReference<ElementType, layoutB> matrixB);

    // matrixB = scalarA * ones + scalarB * matrixB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void ScaleAddUpdate(ElementType scalarA, OnesMatrix, ElementType scalarB, MatrixReference<ElementType, layout> matrixB);

    // matrixB = matrixA + scalarB * matrixB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ScaleAddUpdate(One, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, MatrixReference<ElementType, layoutB> matrixB);

    // matrixB = scalarA * matrixA + scalarB * matrixB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, MatrixReference<ElementType, layoutB> matrixB);
    /// @}

    /// @{
    /// <summary>
    /// Performs the linear operation output = scalarA * matrixA + scalarB * matrixB.
    /// </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="layoutA"> MatrixA layout. </typeparam>
    /// <typeparam name="layoutB"> MatrixB layout. </typeparam>
    /// <typeparam name="outputLayout"> Output matrix layout. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first matrix: either a real number or One().</param>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="scalarB"> The scalar that multiplies the second matrix: either a real number or One() </param>
    /// <param name="matrixB"> The second matrix, which is updated. </param>
    /// <param name="output"> A matrix used to store the result. </param>

    // output = scalarA * matrixA + matrixB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void ScaleAddSet(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, One, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output);

    // output = matrixA + scalarB * matrixB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void ScaleAddSet(One, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output);

    // output = scalarA * matrixA + scalarB * matrixB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void ScaleAddSet(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output);
    /// @}

    /// <summary> Sums each row of a matrix and stores the results in a column vector. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="vector"> The vector used to store the result. </param>
    template <typename ElementType, MatrixLayout layout>
    void RowwiseSum(ConstMatrixReference<ElementType, layout> matrix, ColumnVectorReference<ElementType> vector);

    /// <summary> Sums each column of a matrix and stores the results in a row vector. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="vector"> The vector used to store the result. </param>
    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseSum(ConstMatrixReference<ElementType, layout> matrix, RowVectorReference<ElementType> vector);

    /// <summary> Generalized matrix vector multiplication, vectorB = scalarA * matrix * vectorA + scalarB * vectorB. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the matrix. </param>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="vectorA"> The column vector that multiplies the matrix from the right. </param>
    /// <param name="scalarB"> The scalar that multiplies vectorB. </param>
    /// <param name="vectorB"> A column vector, multiplied by scalarB and used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB);

    /// <summary> Generalized (left-side) matrix row-vector multiplication, vectorB = scalarA * vectorA * matrix + scalarB * vectorB. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the matrix. </param>
    /// <param name="vectorA"> The row vector that multiplies the matrix from the left. </param>
    /// <param name="matrix"> The matrix. </param>
    /// <param name="scalarB"> The scalar that multiplies vectorB. </param>
    /// <param name="vectorB"> A row vector, multiplied by scalarB and used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB);

    /// <summary> Generalized matrix matrix multiplication, matrixC = scalarA * matrixA * matrixB + scalarC * matrixC. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
    /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first matrix. </param>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="matrixB"> The second matrix. </param>
    /// <param name="scalarC"> The scalar that multiplies the third matrix. </param>
    /// <param name="matrixC"> A third matrix, multiplied by scalarC and used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC);

    /// <summary> Matrix matrix element wise multiplication, matrixC = matrixA .* matrixB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
    /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="matrixB"> The second matrix. </param>
    /// <param name="matrixC"> A matrix used to store the result in the layout of first matrix. </param>
    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ElementwiseMultiplySet(ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC);

    /// <summary> Replaces the rows of the input matrix with their cumulative sums. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    template <typename ElementType, MatrixLayout layout>
    void RowwiseCumulativeSumUpdate(MatrixReference<ElementType, layout> matrix);

    /// <summary> Replaces the columns of the input matrix with their cumulative sums. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseCumulativeSumUpdate(MatrixReference<ElementType, layout> matrix);

    /// <summary> Replaces the rows of the input matrix with their consecutive differences. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    template <typename ElementType, MatrixLayout layout>
    void RowwiseConsecutiveDifferenceUpdate(MatrixReference<ElementType, layout> matrix);

    /// <summary> Replaces the columns of the input matrix with their consecutive differences. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="matrix"> The matrix. </param>
    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseConsecutiveDifferenceUpdate(MatrixReference<ElementType, layout> matrix);

    namespace Internal
    {
        template <ImplementationType type>
        struct MatrixOperations
        {};

        template <>
        struct MatrixOperations<ImplementationType::native>
        {
            static std::string GetImplementationName() { return "Native"; }

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC);
        };

#ifdef USE_BLAS
        template <>
        struct MatrixOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBLAS"; }

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, const ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC);
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
