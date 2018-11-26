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

#include <ostream>
#include <string>

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

    /// <summary> Returns the matrix transpose. </summary>
    ///
    /// <param name="matrix"> The const matrix reference to transpose. </param>
    ///
    /// <returns> The transpose. </returns>
    template <typename ElementType, MatrixLayout layout>
    auto Transpose(ConstMatrixReference<ElementType, layout> matrix)
    {
        return matrix.Transpose();
    }

    /// <summary> Returns the matrix transpose. </summary>
    ///
    /// <param name="matrix"> The const matrix reference to transpose. </param>
    ///
    /// <returns> The transpose. </returns>
    template <typename ElementType, MatrixLayout layout>
    auto Transpose(MatrixReference<ElementType, layout> matrix)
    {
        return matrix.Transpose();
    }

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
    struct OnesMatrix
    {};

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

    /// <summary> Performs a rank-one update of the form matrix = matrix + scalar * vectorA * vectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="vectorA"> The column vector. </param>
    /// <param name="vectorB"> The row vector. </param>
    /// <param name="matrix"> The matrix begin updates. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void RankOneUpdate(ElementType scalar, ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix);

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
    /// <typeparam name="layoutC"> Matrix layout of result matrix. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first matrix. </param>
    /// <param name="matrixA"> The first matrix. </param>
    /// <param name="matrixB"> The second matrix. </param>
    /// <param name="scalarC"> The scalar that multiplies the third matrix. </param>
    /// <param name="matrixC"> A third matrix, multiplied by scalarC and used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout layoutC>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutC> matrixC);

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
            static void RankOneUpdate(ElementType scalar, ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix);

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout layoutC>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutC> matrixC);
        };

#ifdef USE_BLAS
        template <>
        struct MatrixOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBLAS"; }

            template <typename ElementType, MatrixLayout layout>
            static void RankOneUpdate(ElementType scalar, ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix);

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layout>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, const ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB);

            template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout layoutC>
            static void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutC> matrixC);
        };

#else
        template <>
        struct MatrixOperations<ImplementationType::openBlas> : public MatrixOperations<ImplementationType::native>
        {
        };

#endif // USE_BLAS
    } // namespace Internal
} // namespace math
} // namespace ell

#pragma region implementation

#include "../include/VectorOperations.h"

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>
#include <utilities/include/Logger.h>

namespace ell
{
namespace math
{
    template <typename ElementType, MatrixLayout layout>
    void Print(ConstMatrixReference<ElementType, layout> M, std::ostream& stream, size_t indent, size_t maxRows, size_t maxElementsPerRow)
    {
        using namespace logging;

        stream << std::string(indent, ' ') << "{";
        if (M.NumRows() > 0)
        {
            Print(M.GetRow(0), stream, 1, maxElementsPerRow);
        }

        if (M.NumRows() <= maxRows)
        {
            for (size_t i = 1; i < M.NumRows(); ++i)
            {
                stream << "," << EOL;
                Print(M.GetRow(i), stream, indent + 2, maxElementsPerRow);
            }
        }
        else
        {
            for (size_t i = 1; i < maxRows - 2; ++i)
            {
                stream << "," << EOL;
                Print(M.GetRow(i), stream, indent + 2, maxElementsPerRow);
            }
            stream << "," << EOL
                   << std::string(indent + 2, ' ') << "...," << EOL;
            Print(M.GetRow(M.NumRows() - 1), stream, indent + 2, maxElementsPerRow);
        }
        stream << " }" << EOL;
    }

    template <typename ElementType, MatrixLayout layout>
    std::ostream& operator<<(std::ostream& stream, ConstMatrixReference<ElementType, layout> M)
    {
        Print(M, stream);
        return stream;
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator+=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        AddUpdate(static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void operator+=(MatrixReference<ElementType, layoutB> matrixB, ConstMatrixReference<ElementType, layoutA> matrixA)
    {
        AddUpdate(matrixA, matrixB);
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator-=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        AddUpdate(-static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void operator-=(MatrixReference<ElementType, layoutB> matrixB, ConstMatrixReference<ElementType, layoutA> matrixA)
    {
        ScaleAddUpdate(static_cast<ElementType>(-1), matrixA, One(), matrixB);
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator*=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        ScaleUpdate(static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator/=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        DEBUG_THROW(scalar == 0, utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero"));

        ScaleUpdate(1 / static_cast<MatrixElementType>(scalar), matrix);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void AddUpdate(ElementType scalar, MatrixReference<ElementType, layout> matrix)
    {
        if (scalar == 0)
        {
            return;
        }
        if (matrix.IsContiguous())
        {
            Internal::VectorOperations<implementation>::AddUpdate(scalar, matrix.ReferenceAsVector());
        }
        else
        {
            for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
            {
                Internal::VectorOperations<implementation>::AddUpdate(scalar, matrix.GetMajorVector(i));
            }
        }
    }

    namespace Internal
    {
        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void AddUpdateAsVectors(ConstMatrixReference<ElementType, layout> matrixA, MatrixReference<ElementType, layout> matrixB)
        {
            if (matrixA.IsContiguous() && matrixB.IsContiguous())
            {
                Internal::VectorOperations<implementation>::AddUpdate(matrixA.ReferenceAsVector(), matrixB.ReferenceAsVector());
            }
            else
            {
                for (size_t i = 0; i < matrixA.GetMinorSize(); ++i)
                {
                    Internal::VectorOperations<implementation>::AddUpdate(matrixA.GetMajorVector(i), matrixB.GetMajorVector(i));
                }
            }
        }

        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void AddUpdateAsVectors(ConstMatrixReference<ElementType, layout> matrixA, MatrixReference<ElementType, TransposeMatrixLayout<layout>::value> matrixB)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::AddUpdate(matrixA.GetRow(i), matrixB.GetRow(i));
            }
        }
    } // namespace Internal

    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void AddUpdate(ConstMatrixReference<ElementType, layoutA> matrixA, MatrixReference<ElementType, layoutB> matrixB)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns(), "Incompatible matrix sizes.");

        Internal::AddUpdateAsVectors<implementation>(matrixA, matrixB);
    }

    namespace Internal
    {
        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void AddSetAsVectors(ElementType scalar, ConstMatrixReference<ElementType, layout> matrix, MatrixReference<ElementType, layout> output)
        {
            if (matrix.IsContiguous() && output.IsContiguous())
            {
                Internal::VectorOperations<implementation>::AddSet(scalar, matrix.ReferenceAsVector(), output.ReferenceAsVector());
            }
            else
            {
                for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
                {
                    Internal::VectorOperations<implementation>::AddSet(scalar, matrix.GetMajorVector(i), output.GetMajorVector(i));
                }
            }
        }

        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void AddSetAsVectors(ElementType scalar, ConstMatrixReference<ElementType, layout> matrix, MatrixReference<ElementType, TransposeMatrixLayout<layout>::value> output)
        {
            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::AddSet(scalar, matrix.GetRow(i), output.GetRow(i));
            }
        }

        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void AddSetAsVectors(ConstMatrixReference<ElementType, layout> matrixA, ConstMatrixReference<ElementType, layout> matrixB, MatrixReference<ElementType, layout> output)
        {
            if (matrixA.IsContiguous() && matrixB.IsContiguous() && output.IsContiguous())
            {
                Internal::VectorOperations<implementation>::AddSet(matrixA.ReferenceAsVector(), matrixB.ReferenceAsVector(), output.ReferenceAsVector());
            }
            else
            {
                for (size_t i = 0; i < matrixA.GetMinorSize(); ++i)
                {
                    Internal::VectorOperations<implementation>::AddSet(matrixA.GetMajorVector(i), matrixB.GetMajorVector(i), output.GetMajorVector(i));
                }
            }
        }

        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void AddSetAsVectors(ConstMatrixReference<ElementType, layout> matrixA, ConstMatrixReference<ElementType, layout> matrixB, MatrixReference<ElementType, TransposeMatrixLayout<layout>::value> output)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::AddSet(matrixA.GetRow(i), matrixB.GetRow(i), output.GetRow(i));
            }
        }

        template <ImplementationType implementation, typename ElementType, MatrixLayout layout, MatrixLayout outputLayout>
        void AddSetAsVectors(ConstMatrixReference<ElementType, layout> matrixA, ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> matrixB, MatrixReference<ElementType, outputLayout> output)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::AddSet(matrixA.GetRow(i), matrixB.GetRow(i), output.GetRow(i));
            }
        }
    } // namespace Internal

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout, MatrixLayout outputLayout>
    void AddSet(ElementType scalar, ConstMatrixReference<ElementType, layout> matrix, MatrixReference<ElementType, outputLayout> output)
    {
        DEBUG_CHECK_SIZES(matrix.NumRows() != output.NumRows() || matrix.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

        if (scalar == 0)
        {
            output.CopyFrom(matrix);
        }
        else
        {
            Internal::AddSetAsVectors<implementation>(scalar, matrix, output);
        }
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void AddSet(ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixA.NumRows() != output.NumRows() || matrixA.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

        Internal::AddSetAsVectors<implementation>(matrixA, matrixB, output);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void ScaleUpdate(ElementType scalar, MatrixReference<ElementType, layout> matrix)
    {
        if (scalar == 0)
        {
            matrix.Reset();
        }
        else if (scalar == 1)
        {
            return;
        }
        else if (matrix.IsContiguous())
        {
            Internal::VectorOperations<implementation>::ScaleUpdate(scalar, matrix.ReferenceAsVector());
        }
        else
        {
            for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
            {
                Internal::VectorOperations<implementation>::ScaleUpdate(scalar, matrix.GetMajorVector(i));
            }
        }
    }

    // implementations of ScaleSet using the equivalent vector operation
    namespace Internal
    {
        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void ScaleSetAsVectors(ElementType scalar, ConstMatrixReference<ElementType, layout> matrix, MatrixReference<ElementType, layout> output)
        {
            if (matrix.IsContiguous() && output.IsContiguous())
            {
                Internal::VectorOperations<implementation>::ScaleSet(scalar, matrix.ReferenceAsVector(), output.ReferenceAsVector());
            }
            else
            {
                for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
                {
                    Internal::VectorOperations<implementation>::ScaleSet(scalar, matrix.GetMajorVector(i), output.GetMajorVector(i));
                }
            }
        }

        template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
        void ScaleSetAsVectors(ElementType scalar, ConstMatrixReference<ElementType, layout> matrix, MatrixReference<ElementType, TransposeMatrixLayout<layout>::value> output)
        {
            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::ScaleSet(scalar, matrix.GetRow(i), output.GetRow(i));
            }
        }
    } // namespace Internal

    template <ImplementationType implementation, typename ElementType, MatrixLayout matrixLayout, MatrixLayout outputLayout>
    void ScaleSet(ElementType scalar, ConstMatrixReference<ElementType, matrixLayout> matrix, MatrixReference<ElementType, outputLayout> output)
    {
        DEBUG_CHECK_SIZES(matrix.NumRows() != output.NumRows() || matrix.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

        if (scalar == 0)
        {
            output.Reset();
        }
        else if (scalar == 1)
        {
            output.CopyFrom(matrix);
        }
        else
        {
            Internal::ScaleSetAsVectors<implementation>(scalar, matrix, output);
        }
    }

    // implementations of ScaleAddUpdate using the equivalent vector operation
    namespace Internal
    {
        template <ImplementationType implementation, typename ElementType, typename scalarAType, typename scalarBType, MatrixLayout layout>
        void ScaleAddUpdateAsVectors(scalarAType scalarA, ConstMatrixReference<ElementType, layout> matrixA, scalarBType scalarB, MatrixReference<ElementType, layout> matrixB)
        {
            if (matrixA.IsContiguous() && matrixB.IsContiguous())
            {
                Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, matrixA.ReferenceAsVector(), scalarB, matrixB.ReferenceAsVector());
            }
            else
            {
                for (size_t i = 0; i < matrixA.GetMinorSize(); ++i)
                {
                    Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, matrixA.GetMajorVector(i), scalarB, matrixB.GetMajorVector(i));
                }
            }
        }

        template <ImplementationType implementation, typename ElementType, typename scalarAType, typename scalarBType, MatrixLayout layout>
        void ScaleAddUpdateAsVectors(scalarAType scalarA, ConstMatrixReference<ElementType, layout> matrixA, scalarBType scalarB, MatrixReference<ElementType, TransposeMatrixLayout<layout>::value> matrixB)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, matrixA.GetRow(i), scalarB, matrixB.GetRow(i));
            }
        }
    } // namespace Internal

    // matrixB += scalarA * matrixA
    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, One, MatrixReference<ElementType, layoutB> matrixB)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns(), "Incompatible matrix sizes.");

        if (scalarA == 0)
        {
            return;
        }
        else if (scalarA == 1)
        {
            AddUpdate<implementation>(matrixA, matrixB);
        }
        else
        {
            Internal::ScaleAddUpdateAsVectors<implementation>(scalarA, matrixA, One(), matrixB);
        }
    }

    // matrixB = scalarA * ones + scalarB * matrixB
    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void ScaleAddUpdate(ElementType scalarA, OnesMatrix, ElementType scalarB, MatrixReference<ElementType, layout> matrixB)
    {
        if (scalarA == 0)
        {
            ScaleUpdate<implementation>(scalarB, matrixB);
        }
        else if (scalarB == 0)
        {
            matrixB.Fill(scalarA);
        }
        else if (scalarB == 1)
        {
            AddUpdate<implementation>(scalarA, matrixB);
        }
        else if (matrixB.IsContiguous())
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, OnesVector(), scalarB, matrixB.ReferenceAsVector());
        }
        else
        {
            for (size_t i = 0; i < matrixB.GetMinorSize(); ++i)
            {
                Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, OnesVector(), scalarB, matrixB.GetMajorVector(i));
            }
        }
    }

    // matrixB = matrixA + scalarB * matrixB
    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ScaleAddUpdate(One, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, MatrixReference<ElementType, layoutB> matrixB)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns(), "Incompatible matrix sizes.");

        if (scalarB == 0)
        {
            matrixB.CopyFrom(matrixA);
        }
        else if (scalarB == 1)
        {
            AddUpdate<implementation>(matrixA, matrixB);
        }
        else
        {
            Internal::ScaleAddUpdateAsVectors<implementation>(One(), matrixA, scalarB, matrixB);
        }
    }

    // matrixB = scalarA * matrixA + scalarB * matrixB
    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, MatrixReference<ElementType, layoutB> matrixB)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns(), "Incompatible matrix sizes.");

        if (scalarA == 0)
        {
            ScaleUpdate<implementation>(scalarB, matrixB);
        }
        else if (scalarA == 1)
        {
            ScaleAddUpdate<implementation>(One(), matrixA, scalarB, matrixB);
        }
        else if (scalarB == 0)
        {
            Internal::ScaleSetAsVectors<implementation>(scalarA, matrixA, matrixB);
        }
        else if (scalarB == 1)
        {
            Internal::ScaleAddUpdateAsVectors<implementation>(scalarA, matrixA, One(), matrixB);
        }
        else
        {
            Internal::ScaleAddUpdateAsVectors<implementation>(scalarA, matrixA, scalarB, matrixB);
        }
    }

    // implementations of ScaleAddSet using the equivalent vector operation
    namespace Internal
    {
        template <ImplementationType implementation, typename ElementType, typename scalarAType, typename scalarBType, MatrixLayout layout>
        void ScaleAddSetAsVectors(scalarAType scalarA, ConstMatrixReference<ElementType, layout> matrixA, scalarBType scalarB, ConstMatrixReference<ElementType, layout> matrixB, MatrixReference<ElementType, layout> output)
        {
            if (matrixA.IsContiguous() && matrixB.IsContiguous() && output.IsContiguous())
            {
                Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, matrixA.ReferenceAsVector(), scalarB, matrixB.ReferenceAsVector(), output.ReferenceAsVector());
            }
            else
            {
                for (size_t i = 0; i < matrixA.GetMinorSize(); ++i)
                {
                    Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, matrixA.GetMajorVector(i), scalarB, matrixB.GetMajorVector(i), output.GetMajorVector(i));
                }
            }
        }

        template <ImplementationType implementation, typename ElementType, typename scalarAType, typename scalarBType, MatrixLayout layout>
        void ScaleAddSetAsVectors(scalarAType scalarA, ConstMatrixReference<ElementType, layout> matrixA, scalarBType scalarB, ConstMatrixReference<ElementType, layout> matrixB, MatrixReference<ElementType, TransposeMatrixLayout<layout>::value> output)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, matrixA.GetRow(i), scalarB, matrixB.GetRow(i), output.GetRow(i));
            }
        }

        template <ImplementationType implementation, typename ElementType, typename scalarAType, typename scalarBType, MatrixLayout layout, MatrixLayout outputLayout>
        void ScaleAddSetAsVectors(scalarAType scalarA, ConstMatrixReference<ElementType, layout> matrixA, scalarBType scalarB, ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> matrixB, MatrixReference<ElementType, outputLayout> output)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, matrixA.GetRow(i), scalarB, matrixB.GetRow(i), output.GetRow(i));
            }
        }
    } // namespace Internal

    // output = scalarA * matrixA + matrixB
    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void ScaleAddSet(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, One, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixA.NumRows() != output.NumRows() || matrixA.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

        if (scalarA == 0)
        {
            output.CopyFrom(matrixB);
        }
        else if (scalarA == 1)
        {
            Internal::AddSetAsVectors<implementation>(matrixA, matrixB, output);
        }
        else
        {
            Internal::ScaleAddSetAsVectors<implementation>(scalarA, matrixA, One(), matrixB, output);
        }
    }

    //// output = matrixA + scalarB * matrixB
    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void ScaleAddSet(One, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixA.NumRows() != output.NumRows() || matrixA.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

        if (scalarB == 0)
        {
            output.CopyFrom(matrixA);
        }
        else if (scalarB == 1)
        {
            Internal::AddSetAsVectors<implementation>(matrixA, matrixB, output);
        }
        else
        {
            Internal::ScaleAddSetAsVectors<implementation>(One(), matrixA, scalarB, matrixB, output);
        }
    }

    // output = scalarA * matrixA + scalarB * matrixB
    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout outputLayout>
    void ScaleAddSet(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType scalarB, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, outputLayout> output)
    {
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixA.NumRows() != output.NumRows() || matrixA.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

        if (scalarA == 0)
        {
            ScaleSet<implementation>(scalarB, matrixB, output);
        }
        else if (scalarA == 1)
        {
            ScaleAddSet<implementation>(One(), matrixA, scalarB, matrixB, output);
        }
        else if (scalarB == 0)
        {
            Internal::ScaleSetAsVectors<implementation>(scalarA, matrixA, output);
        }
        else if (scalarB == 1)
        {
            Internal::ScaleAddSetAsVectors<implementation>(scalarA, matrixA, One(), matrixB, output);
        }
        else
        {
            Internal::ScaleAddSetAsVectors<implementation>(scalarA, matrixA, scalarB, matrixB, output);
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void RowwiseSum(ConstMatrixReference<ElementType, layout> matrix, ColumnVectorReference<ElementType> vector)
    {
        DEBUG_CHECK_SIZES(vector.Size() != matrix.NumRows(), "Incompatible matrix vector sizes.");

        math::ColumnVector<ElementType> ones(matrix.NumColumns());
        ones.Fill(1);

        MultiplyScaleAddUpdate(static_cast<ElementType>(1), matrix, ones, static_cast<ElementType>(0), vector);
    }

    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseSum(ConstMatrixReference<ElementType, layout> matrix, RowVectorReference<ElementType> vector)
    {
        DEBUG_CHECK_SIZES(vector.Size() != matrix.NumColumns(), "Incompatible matrix vector sizes.");

        math::RowVector<ElementType> ones(matrix.NumRows());
        ones.Fill(1);

        MultiplyScaleAddUpdate(static_cast<ElementType>(1), ones, matrix, static_cast<ElementType>(0), vector);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void RankOneUpdate(ElementType scalar, ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix)
    {
        DEBUG_CHECK_SIZES(vectorA.Size() != matrix.NumRows() || vectorB.Size() != matrix.NumColumns(), "Incompatible matrix vector sizes.");
        Internal::MatrixOperations<implementation>::RankOneUpdate(scalar, vectorA, vectorB, matrix);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB)
    {
        DEBUG_CHECK_SIZES(matrix.NumColumns() != vectorA.Size() || matrix.NumRows() != vectorB.Size(), "Incompatible matrix vector sizes.");

        Internal::MatrixOperations<implementation>::MultiplyScaleAddUpdate(scalarA, matrix, vectorA, scalarB, vectorB);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB)
    {
        DEBUG_CHECK_SIZES(matrix.NumRows() != vectorA.Size() || matrix.NumColumns() != vectorB.Size(), "Incompatible matrix vector sizes.");

        Internal::MatrixOperations<implementation>::MultiplyScaleAddUpdate(scalarA, vectorA, matrix, scalarB, vectorB);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout layoutC>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutC> matrixC)
    {
        DEBUG_CHECK_SIZES(matrixA.NumColumns() != matrixB.NumRows() || matrixA.NumRows() != matrixC.NumRows() || matrixB.NumColumns() != matrixC.NumColumns(), "Incompatible matrix sizes.");

        Internal::MatrixOperations<implementation>::MultiplyScaleAddUpdate(scalarA, matrixA, matrixB, scalarC, matrixC);
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ElementwiseMultiplySet(ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC)
    {
        for (size_t i = 0; i < matrixA.NumRows(); ++i)
        {
            ElementwiseMultiplySet(matrixA.GetRow(i), matrixB.GetRow(i), matrixC.GetRow(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void RowwiseCumulativeSumUpdate(MatrixReference<ElementType, layout> matrix)
    {
        for (size_t i = 0; i < matrix.NumRows(); ++i)
        {
            CumulativeSumUpdate(matrix.GetRow(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseCumulativeSumUpdate(MatrixReference<ElementType, layout> matrix)
    {
        for (size_t i = 0; i < matrix.NumColumns(); ++i)
        {
            CumulativeSumUpdate(matrix.GetColumn(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void RowwiseConsecutiveDifferenceUpdate(MatrixReference<ElementType, layout> matrix)
    {
        for (size_t i = 0; i < matrix.NumRows(); ++i)
        {
            ConsecutiveDifferenceUpdate(matrix.GetRow(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseConsecutiveDifferenceUpdate(MatrixReference<ElementType, layout> matrix)
    {
        for (size_t i = 0; i < matrix.NumColumns(); ++i)
        {
            ConsecutiveDifferenceUpdate(matrix.GetColumn(i));
        }
    }

    //
    // Native implementations of operations
    //

    namespace Internal
    {
        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::native>::RankOneUpdate(ElementType scalar, ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix)
        {
            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                for (size_t j = 0; j < matrix.NumColumns(); ++j)
                {
                    matrix(i, j) += scalar * vectorA[i] * vectorB[j];
                }
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::native>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB)
        {
            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                auto row = matrix.GetRow(i);
                vectorB[i] = scalarA * Dot(row, vectorA) + scalarB * vectorB[i];
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::native>::MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB)
        {
            MultiplyScaleAddUpdate(scalarA, matrix.Transpose(), vectorA.Transpose(), scalarB, vectorB.Transpose());
        }

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout layoutC>
        void MatrixOperations<ImplementationType::native>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarB, MatrixReference<ElementType, layoutC> matrixC)
        {
            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                for (size_t j = 0; j < matrixB.NumColumns(); ++j)
                {
                    auto row = matrixA.GetRow(i);
                    auto column = matrixB.GetColumn(j);
                    matrixC(i, j) = scalarA * Dot(row, column) + scalarB * matrixC(i, j);
                }
            }
        }

#if defined(USE_BLAS)
        //
        // OpenBLAS implementations of operations
        //

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::RankOneUpdate(ElementType scalar, ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix)
        {
            Blas::Ger(matrix.GetLayout(), static_cast<int>(matrix.NumRows()), static_cast<int>(matrix.NumColumns()), scalar, vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetConstDataPointer(), static_cast<int>(vectorB.GetIncrement()), matrix.GetDataPointer(), static_cast<int>(matrix.GetIncrement()));
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB)
        {
            Blas::Gemv(matrix.GetLayout(), MatrixTranspose::noTranspose, static_cast<int>(matrix.NumRows()), static_cast<int>(matrix.NumColumns()), scalarA, matrix.GetConstDataPointer(), static_cast<int>(matrix.GetIncrement()), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), scalarB, vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB)
        {
            MultiplyScaleAddUpdate(scalarA, matrix.Transpose(), vectorA.Transpose(), scalarB, vectorB.Transpose());
        }

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB, MatrixLayout layoutC>
        void MatrixOperations<ImplementationType::openBlas>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarB, MatrixReference<ElementType, layoutC> matrixC)
        {
            MatrixLayout order = matrixC.GetLayout();
            MatrixTranspose transposeA = matrixA.GetLayout() == order ? MatrixTranspose::noTranspose : MatrixTranspose::transpose;
            MatrixTranspose transposeB = matrixB.GetLayout() == order ? MatrixTranspose::noTranspose : MatrixTranspose::transpose;

            Blas::Gemm(order, transposeA, transposeB, static_cast<int>(matrixA.NumRows()), static_cast<int>(matrixB.NumColumns()), static_cast<int>(matrixA.NumColumns()), scalarA, matrixA.GetConstDataPointer(), static_cast<int>(matrixA.GetIncrement()), matrixB.GetConstDataPointer(), static_cast<int>(matrixB.GetIncrement()), scalarB, matrixC.GetDataPointer(), static_cast<int>(matrixC.GetIncrement()));
        }
#endif
    } // namespace Internal
} // namespace math
} // namespace ell

#pragma endregion implementation
