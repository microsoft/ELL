////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VectorOperations.h"

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    template <typename ElementType, MatrixLayout layout>
    void Print(ConstMatrixReference<ElementType, layout> M, std::ostream& stream, size_t indent, size_t maxRows, size_t maxElementsPerRow)
    {
        stream << std::string(indent, ' ') << "{";
        if (M.NumRows() > 0)
        {
            Print(M.GetRow(0), stream, 1, maxElementsPerRow);
        }

        if (M.NumRows() <= maxRows)
        {
            for (size_t i = 1; i < M.NumRows(); ++i)
            {
                stream << ",\n";
                Print(M.GetRow(i), stream, indent + 2, maxElementsPerRow);
            }
        }
        else
        {
            for (size_t i = 1; i < maxRows - 2; ++i)
            {
                stream << ",\n";
                Print(M.GetRow(i), stream, indent + 2, maxElementsPerRow);
            }
            stream << ",\n"
                << std::string(indent + 2, ' ') << "...,\n";
            Print(M.GetRow(M.NumRows() - 1), stream, indent + 2, maxElementsPerRow);
        }
        stream << " }\n";
    }

    template <typename ElementType, MatrixLayout layout>
    std::ostream& operator<<(std::ostream& stream, ConstMatrixReference<ElementType, layout> M)
    {
        Print(M, stream);
        return stream;
    }

    template <typename ElementType, MatrixLayout layout>
    void Add(ElementType scalar, MatrixReference<ElementType, layout> matrix)
    {
        for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
        {
            Add(scalar, matrix.GetMajorVector(i));
        }
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator+=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        Add(static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator-=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        Add(-static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void Add(ElementType value1, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType value2, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC)
    {
        Internal::MatrixOperations<ImplementationType::openBlas>::Add(value1, matrixA, value2, matrixB, matrixC);
    }

    template <typename ElementType, MatrixLayout layout>
    void RowwiseSum(ConstMatrixReference<ElementType, layout> matrix, VectorReference<ElementType, VectorOrientation::column> vector)
    {
        if (vector.Size() != matrix.NumRows())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible result size.");
        }

        math::ColumnVector<ElementType> ones(matrix.NumColumns());
        ones.Fill(1.0);

        Multiply(static_cast<ElementType>(1), matrix, ones, static_cast<ElementType>(0), vector);
    }

    template <typename ElementType, MatrixLayout layout>
    void ColumnwiseSum(ConstMatrixReference<ElementType, layout> matrix, VectorReference<ElementType, VectorOrientation::row> vector)
    {
        if (vector.Size() != matrix.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible result size.");
        }

        math::RowVector<ElementType> ones(matrix.NumRows());
        ones.Fill(1.0);

        Multiply(static_cast<ElementType>(1), ones, matrix, static_cast<ElementType>(0), vector);
    }

    template <typename ElementType, MatrixLayout layout>
    void Multiply(ElementType scalar, MatrixReference<ElementType, layout> matrix)
    {
        Internal::MatrixOperations<ImplementationType::openBlas>::Multiply(scalar, matrix);
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator*=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        Multiply(static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename MatrixElementType, MatrixLayout layout, typename ScalarElementType, utilities::IsFundamental<ScalarElementType>>
    void operator/=(MatrixReference<MatrixElementType, layout> matrix, ScalarElementType scalar)
    {
        if (scalar == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero");
        }
        Multiply(1/static_cast<MatrixElementType>(scalar), matrix);
    }

    template <typename ElementType, MatrixLayout layout>
    void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ElementType scalarB, VectorReference<ElementType, VectorOrientation::column> vectorB)
    {
        Internal::MatrixOperations<ImplementationType::openBlas>::Multiply(scalarA, matrix, vectorA, scalarB, vectorB);
    }

    template <typename ElementType, MatrixLayout layout>
    void Multiply(ElementType scalarA, ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, VectorReference<ElementType, VectorOrientation::row> vectorB)
    {
        Internal::MatrixOperations<ImplementationType::openBlas>::Multiply(scalarA, vectorA, matrix, scalarB, vectorB);
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC)
    {
        Internal::MatrixOperations<ImplementationType::openBlas>::Multiply(scalarA, matrixA, matrixB, scalarC, matrixC);
    }

    template <typename ElementType, MatrixLayout layout>
    void MultiplyAdd(ElementType scalarA, ElementType scalarB, MatrixReference<ElementType, layout> matrix)
    {
        if (scalarB == 0)
        {
            math::Multiply(scalarA, matrix);
        }
        else
        {
            for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
            {
                auto interval = matrix.GetMajorVector(i);
                math::MultiplyAdd(scalarA, scalarB, interval);
            }
        }
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void ElementwiseMultiply(ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC)
    {
        DEBUG_THROW(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixB.NumRows() != matrixC.NumRows() || matrixB.NumColumns() != matrixC.NumColumns(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes."));

        for (size_t i = 0; i < matrixA.NumRows(); ++i)
        {
            math::ElementwiseMultiply(matrixA.GetRow(i), matrixB.GetRow(i), matrixC.GetRow(i));
        }
    }

    //
    // Native implementations of operations
    //

    namespace Internal
    {
        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        void MatrixOperations<ImplementationType::native>::Add(ElementType value1, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType value2, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC)
        {
            DEBUG_THROW(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixB.NumRows() != matrixC.NumRows() || matrixB.NumColumns() != matrixC.NumColumns(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes."));

            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                VectorOperations<ImplementationType::native>::Add(value1, matrixA.GetRow(i), matrixC.GetRow(i));
                VectorOperations<ImplementationType::native>::Add(value2, matrixB.GetRow(i), matrixC.GetRow(i));
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::native>::Multiply(ElementType scalar, MatrixReference<ElementType, layout> matrix)
        {
            for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
            {
                VectorOperations<ImplementationType::native>::Multiply(scalar, matrix.GetMajorVector(i));
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::native>::Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ElementType scalarB, VectorReference<ElementType, VectorOrientation::column> vectorB)
        {
            if (matrix.NumRows() != vectorB.Size() || matrix.NumColumns() != vectorA.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix and vectors sizes.");
            }

            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                auto row = matrix.GetRow(i);
                vectorB[i] = scalarA * Dot(row, vectorA) + scalarB * vectorB[i];
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::native>::Multiply(ElementType scalarA, ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, VectorReference<ElementType, VectorOrientation::row> vectorB)
        {
            Multiply(scalarA, matrix.Transpose(), vectorA.Transpose(), scalarB, vectorB.Transpose());
        }

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        void MatrixOperations<ImplementationType::native>::Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarB, MatrixReference<ElementType, layoutA> matrixC)
        {
            if (matrixA.NumColumns() != matrixB.NumRows() || matrixA.NumRows() != matrixC.NumRows() || matrixB.NumColumns() != matrixC.NumColumns())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes.");
            }

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

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        void MatrixOperations<ImplementationType::openBlas>::Add(ElementType value1, ConstMatrixReference<ElementType, layoutA> matrixA, ElementType value2, ConstMatrixReference<ElementType, layoutB> matrixB, MatrixReference<ElementType, layoutA> matrixC)
        {
            DEBUG_THROW(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns() || matrixB.NumRows() != matrixC.NumRows() || matrixB.NumColumns() != matrixC.NumColumns(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes."));

            for (size_t i = 0; i < matrixA.NumRows(); ++i)
            {
                VectorOperations<ImplementationType::openBlas>::Add(value1, matrixA.GetRow(i), matrixC.GetRow(i));
                VectorOperations<ImplementationType::openBlas>::Add(value2, matrixB.GetRow(i), matrixC.GetRow(i));
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::Multiply(ElementType scalar, MatrixReference<ElementType, layout> matrix)
        {
            for (size_t i = 0; i < matrix.GetMinorSize(); ++i)
            {
                VectorOperations<ImplementationType::openBlas>::Multiply(scalar, matrix.GetMajorVector(i));
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ElementType scalarB, VectorReference<ElementType, VectorOrientation::column> vectorB)
        {
            if (matrix.NumRows() != vectorB.Size() || matrix.NumColumns() != vectorA.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix and vectors sizes.");
            }

            Blas::Gemv(matrix.GetLayout(), MatrixTranspose::noTranspose, static_cast<int>(matrix.NumRows()), static_cast<int>(matrix.NumColumns()), scalarA, matrix.GetConstDataPointer(), static_cast<int>(matrix.GetIncrement()), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), scalarB, vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::Multiply(ElementType scalarA, ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, VectorReference<ElementType, VectorOrientation::row> vectorB)
        {
            Multiply(scalarA, matrix.Transpose(), vectorA.Transpose(), scalarB, vectorB.Transpose());
        }

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        void MatrixOperations<ImplementationType::openBlas>::Multiply(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarB, MatrixReference<ElementType, layoutA> matrixC)
        {
            if (matrixA.NumColumns() != matrixB.NumRows() || matrixA.NumRows() != matrixC.NumRows() || matrixB.NumColumns() != matrixC.NumColumns())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes.");
            }

            MatrixLayout order = matrixA.GetLayout();
            MatrixTranspose transposeB = MatrixTranspose::noTranspose;
            if (matrixA.GetLayout() != matrixB.GetLayout())
            {
                transposeB = MatrixTranspose::transpose;
            }

            Blas::Gemm(order, MatrixTranspose::noTranspose, transposeB, static_cast<int>(matrixA.NumRows()), static_cast<int>(matrixB.NumColumns()), static_cast<int>(matrixA.NumColumns()), scalarA,
                matrixA.GetConstDataPointer(), static_cast<int>(matrixA.GetIncrement()), matrixB.GetConstDataPointer(), static_cast<int>(matrixB.GetIncrement()), scalarB,
                matrixC.GetDataPointer(), static_cast<int>(matrixC.GetIncrement()));
        }
#endif
    }
}
}