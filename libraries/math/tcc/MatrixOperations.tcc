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
#include "Logger.h"

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
        template<ImplementationType implementation, typename ElementType, MatrixLayout layout>
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
    }

    template<ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
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
    }

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
        DEBUG_CHECK_SIZES(matrixA.NumRows() != matrixB.NumRows() || matrixA.NumColumns() != matrixB.NumColumns()|| matrixA.NumRows() != output.NumRows() || matrixA.NumColumns() != output.NumColumns(), "Incompatible matrix sizes.");

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
    }

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
    }

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
        else if(matrixB.IsContiguous())
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
    }

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
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB)
    {
        DEBUG_CHECK_SIZES(matrix.NumColumns() != vectorA.Size() || matrix.NumRows() != vectorB.Size() , "Incompatible matrix vector sizes.");

        Internal::MatrixOperations<implementation>::MultiplyScaleAddUpdate(scalarA, matrix, vectorA, scalarB, vectorB);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB)
    {
        DEBUG_CHECK_SIZES(matrix.NumRows() != vectorA.Size() || matrix.NumColumns() != vectorB.Size(), "Incompatible matrix vector sizes.");

        Internal::MatrixOperations<implementation>::MultiplyScaleAddUpdate(scalarA, vectorA, matrix, scalarB, vectorB);
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarC, MatrixReference<ElementType, layoutA> matrixC)
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

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        void MatrixOperations<ImplementationType::native>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarB, MatrixReference<ElementType, layoutA> matrixC)
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
        void MatrixOperations<ImplementationType::openBlas>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layout> matrix, ConstColumnVectorReference<ElementType> vectorA, ElementType scalarB, ColumnVectorReference<ElementType> vectorB)
        {
            Blas::Gemv(matrix.GetLayout(), MatrixTranspose::noTranspose, static_cast<int>(matrix.NumRows()), static_cast<int>(matrix.NumColumns()), scalarA, matrix.GetConstDataPointer(), static_cast<int>(matrix.GetIncrement()), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), scalarB, vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        template <typename ElementType, MatrixLayout layout>
        void MatrixOperations<ImplementationType::openBlas>::MultiplyScaleAddUpdate(ElementType scalarA, ConstRowVectorReference<ElementType> vectorA, ConstMatrixReference<ElementType, layout> matrix, ElementType scalarB, RowVectorReference<ElementType> vectorB)
        {
            MultiplyScaleAddUpdate(scalarA, matrix.Transpose(), vectorA.Transpose(), scalarB, vectorB.Transpose());
        }

        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        void MatrixOperations<ImplementationType::openBlas>::MultiplyScaleAddUpdate(ElementType scalarA, ConstMatrixReference<ElementType, layoutA> matrixA, ConstMatrixReference<ElementType, layoutB> matrixB, ElementType scalarB, MatrixReference<ElementType, layoutA> matrixC)
        {
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
