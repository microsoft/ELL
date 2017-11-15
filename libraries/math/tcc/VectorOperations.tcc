////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    template <typename ElementType, VectorOrientation orientation>
    void Print(ConstVectorReference<ElementType, orientation> vector, std::ostream& stream, size_t indent, size_t maxElements)
    {
        DEBUG_THROW(maxElements < 3, utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "cannot specify maxElements below 3."));

        stream << std::string(indent, ' ');
        if (vector.Size() == 0)
        {
            stream << "{ }";
        }
        else if (vector.Size() <= maxElements)
        {
            stream << "{ " << vector[0];
            for (size_t i = 1; i < vector.Size(); ++i)
            {
                stream << ", " << vector[i];
            }
            stream << " }";
        }
        else
        {
            stream << "{ " << vector[0];
            for (size_t i = 1; i < maxElements - 2; ++i)
            {
                stream << ", " << vector[i];
            }
            stream << ", ..., " << vector[vector.Size() - 1] << " }";
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    std::ostream& operator<<(std::ostream& stream, ConstVectorReference<ElementType, orientation> vector)
    {
        Print(vector, stream);
        return stream;
    }

    template <typename ElementType, utilities::IsFundamental<ElementType> concept = true>
    ElementType Square(ElementType x) 
    { 
        return x * x; 
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ElementTransformation<ElementType>>
    {
        return TransformVector(vector, static_cast<ElementTransformation<ElementType>>(Square<ElementType>));
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ElementTransformation<ElementType>>
    {
        return TransformVector(vector, static_cast<ElementTransformation<ElementType>>(std::sqrt));
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ElementTransformation<ElementType>>
    {
        return TransformVector(vector, static_cast<ElementTransformation<ElementType>>(std::abs));
    }

    template <typename ElementType>
    ElementType ScaleFunction<ElementType>::operator()(ElementType x)
    { 
        return x * _value; 
    }

    template <typename ElementType, VectorOrientation orientation>
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ScaleFunction<ElementType>>
    {
        ScaleFunction<ElementType> transformation{ static_cast<ElementType>(scalar) };
        return TransformVector(vector, transformation);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator+=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        AddUpdate(static_cast<VectorElementType>(scalar), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator-=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        AddUpdate(static_cast<VectorElementType>(-scalar), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator*=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        ScaleUpdate(static_cast<VectorElementType>(scalar), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator/=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        if (scalar == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero");
        }
        ScaleUpdate(1/static_cast<VectorElementType>(scalar), vector);
    }

    template <typename ElementType, VectorOrientation orientation>
    void ElementwiseMultiplySet(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB, VectorReference<ElementType, orientation> vectorC)
    {
        DEBUG_THROW(vectorA.Size() != vectorB.Size() || vectorA.Size() != vectorB.Size(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible vector sizes."));

        const ElementType* pVectorAData = vectorA.GetConstDataPointer();
        const ElementType* pVectorBData = vectorB.GetConstDataPointer();

        size_t i = 0;
        const ElementType* end = vectorA.GetConstDataPointer() + vectorA.GetIncrement() * vectorA.Size();

        while (pVectorAData < end)
        {
            vectorC[i++] = (*pVectorAData) * (*pVectorBData);
            pVectorAData += vectorA.GetIncrement();
            pVectorBData += vectorB.GetIncrement();
        }
    }

    template <typename ElementType>
    void InnerProduct(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result)
    {
        Internal::VectorOperations<ImplementationType::openBlas>::InnerProduct(vectorA, vectorB, result);
    }

    template <typename ElementType>
    ElementType Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB)
    {
        ConstVectorReference<ElementType, VectorOrientation::row> rowVector(vectorA.GetConstDataPointer(), vectorA.Size(), vectorA.GetIncrement());
        ConstVectorReference<ElementType, VectorOrientation::column> columnVector(vectorB.GetConstDataPointer(), vectorB.Size(), vectorB.GetIncrement());

        ElementType result;
        InnerProduct(rowVector, columnVector, result);
        return result;
    }

    template <typename ElementType, MatrixLayout layout>
    void OuterProduct(ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ConstVectorReference<ElementType, VectorOrientation::row> vectorB, MatrixReference<ElementType, layout> matrix)
    {
        Internal::VectorOperations<ImplementationType::openBlas>::OuterProduct(vectorA, vectorB, matrix);
    }

    // vector += scalar
    template <typename ElementType, VectorOrientation orientation>
    void AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
    {
        if (scalar == 0)
        {
            return;
        }
        else
        {
            ElementType* pData = vector.GetDataPointer();
            const ElementType* pEnd = vector.GetConstDataPointer() + vector.GetIncrement() * vector.Size();

            while (pData < pEnd)
            {
                (*pData) += scalar;
                pData += vector.GetIncrement();
            }
        }
    }

    // vectorB += vectorA
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
    {
        if (vectorB.Size() != vectorA.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and vectorA are not the same size.");
        }

        Internal::VectorOperations<implementation>::AddUpdate(vectorA, vectorB);
    }

    // output = scalar + vector
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
    {
        if (scalar == 0)
        {
            output.CopyFrom(vector);
        }
        else
        {
            Internal::VectorOperations<implementation>::AddSet(scalar, vector, output);
        }
    }

    // output = vectorA + vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        if (vectorA.Size() != vectorB.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectorA and vectorB are not the same size.");
        }

        Internal::VectorOperations<implementation>::AddSet(vectorA, vectorB, output);
    }

    // vector *= scalar
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
    {
        if (scalar == 1)
        {
            return;
        }
        else if (scalar == 0)
        {
            vector.Reset();
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleUpdate(scalar, vector);
        }
    }

    // output = scalar * vector
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
    {
        if (vector.Size() != output.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vector and output are not the same size.");
        }

        if (scalar == 1)
        {
            output.CopyFrom(vector);
        }
        else if (scalar == 0)
        {
            output.Reset();
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleSet(scalar, vector, output);
        }
    }

    // vectorB += scalarA * vectorA
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB)
    {
        if (vectorB.Size() != vectorA.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and vectorA are not the same size.");
        }

        if (scalarA == 1)
        {
            AddUpdate<implementation>(vectorA, vectorB);
        }
        else if (scalarA == 0)
        {
            return;
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, vectorA, One(), vectorB);
        }
    }

    // vectorB = scalarA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
    {
        if (scalarA == 0)
        {
            ScaleUpdate<implementation>(scalarB, vectorB);
        }
        else if (scalarB == 0)
        {
            vectorB.Fill(scalarA);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, OnesVector(), scalarB, vectorB);
        }
    }

    // vectorB = vectorA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
    {
        if (vectorB.Size() != vectorA.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and vectorA are not the same size.");
        }

        if (scalarB == 0)
        {
            vectorB.CopyFrom(vectorA);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(One(), vectorA, scalarB, vectorB);
        }
    }

    // vectorB =  scalarA * vectorA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
    {
        if (vectorB.Size() != vectorA.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and vectorA are not the same size.");
        }
        
        if (scalarA == 0)
        {
            ScaleUpdate<implementation>(scalarB, vectorB);
        }
        else if (scalarA == 1)
        {
            ScaleAddUpdate<implementation>(One(), vectorA, scalarB, vectorB);
        }
        else if (scalarB == 0)
        {
            ScaleSet<implementation>(scalarA, vectorA, vectorB);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, vectorA, scalarB, vectorB);
        }
    }

    // output = scalarA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        if (vectorB.Size() != output.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and output are not the same size.");
        }

        if (scalarA == 0)
        {
            ScaleSet<implementation>(scalarB, vectorB, output);
        }
        else if (scalarB == 0)
        {
            output.Fill(scalarA);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, OnesVector(), scalarB, vectorB, output);
        }
    }

    // output = scalarA * vectorA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        if (vectorB.Size() != vectorA.Size() || vectorB.Size() != output.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorA, VectorB, output are not the same size.");
        }
        if (scalarA == 0)
        {
            ScaleSet<implementation>(scalarB, vectorB, output);
        }
        else if (scalarB == 0)
        {
            ScaleSet<implementation>(scalarA, vectorA, output);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, vectorA, scalarB, vectorB, output);
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void CumulativeSumUpdate(VectorReference<ElementType, orientation> vector)
    {
        ElementType* pData = vector.GetDataPointer();
        const ElementType* pEnd = pData + vector.GetIncrement() * vector.Size();
        ElementType sum = (*pData);
        pData += vector.GetIncrement();

        while (pData < pEnd)
        {
            sum += (*pData);
            (*pData) = sum;
            pData += vector.GetIncrement();
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void ConsecutiveDifferenceUpdate(VectorReference<ElementType, orientation> vector)
    {
        ElementType* pData = vector.GetDataPointer();
        const ElementType* pEnd = pData + vector.GetIncrement() * vector.Size();
        ElementType previous = (*pData);
        pData += vector.GetIncrement();

        while (pData < pEnd)
        {
            ElementType sum = (*pData);
            (*pData) -= previous;
            previous = sum;
            pData += vector.GetIncrement();
        }
    }

    //
    // NativeVectorOperations
    //
    namespace Internal
    {
        template <typename ElementType>
        void VectorOperations<ImplementationType::native>::InnerProduct(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result)
        {
            if (vectorA.Size() != vectorB.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectorA and vectorB are not the same size.");
            }

            const ElementType* pVectorAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBData = vectorB.GetConstDataPointer();
            const ElementType* pVectorAEnd = pVectorAData + vectorA.GetIncrement() * vectorA.Size();
            result = 0;

            while (pVectorAData < pVectorAEnd)
            {
                result += (*pVectorAData) * (*pVectorBData);
                pVectorAData += vectorA.GetIncrement();
                pVectorBData += vectorB.GetIncrement();
            }
        }

        template <typename ElementType, MatrixLayout layout>
        void VectorOperations<ImplementationType::native>::OuterProduct(ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ConstVectorReference<ElementType, VectorOrientation::row> vectorB, MatrixReference<ElementType, layout> matrix)
        {
            if (vectorA.Size() != matrix.NumRows() || vectorB.Size() != matrix.NumColumns())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "inconsistent vector and matrix sizes");
            }

            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                for (size_t j = 0; j < matrix.NumColumns(); ++j)
                {
                    matrix(i, j) = vectorA[i] * vectorB[j];
                }
            }
        }

        // vectorB += scalarA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
        {
            ElementType* pVectorBData = vectorB.GetDataPointer();
            const ElementType* pVectorAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                (*pVectorBData) += (*pVectorAData);
                pVectorBData += vectorB.GetIncrement();
                pVectorAData += vectorA.GetIncrement();
            }
        }

        // output = scalar + vector
        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            ElementType* pOutputData = output.GetDataPointer();
            const ElementType* pVectorData = vector.GetConstDataPointer();
            const ElementType* pOutputEnd = pOutputData + output.GetIncrement() * output.Size();

            while (pOutputData < pOutputEnd)
            {
                (*pOutputData) = scalar + (*pVectorData);
                pVectorData += vector.GetIncrement();
                pOutputData += output.GetIncrement();
            }
        }

        // output = vectorA + vectorB
        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            ElementType* pOutputData = output.GetDataPointer();
            const ElementType* pVectortAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBData = vectorB.GetConstDataPointer();
            const ElementType* pOutputEnd = pOutputData + output.GetIncrement() * output.Size();

            while (pOutputData < pOutputEnd)
            {
                (*pOutputData) = (*pVectortAData) + (*pVectorBData);
                pVectortAData += vectorA.GetIncrement();
                pVectorBData += vectorB.GetIncrement();
                pOutputData += output.GetIncrement();
            }
        }

        // vector *= scalar
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
        {
            ElementType* pData = vector.GetDataPointer();
            const ElementType* pEnd = pData + vector.GetIncrement() * vector.Size();

            while (pData < pEnd)
            {
                (*pData) *= scalar;
                pData += vector.GetIncrement();
            }
        }

        // output = scalarA * vectorA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            ElementType* pOutputData = output.GetDataPointer();
            const ElementType* pVectorData = vector.GetConstDataPointer();
            const ElementType* pOutputEnd = pOutputData + output.GetIncrement() * output.Size();

            while (pOutputData < pOutputEnd)
            {
                (*pOutputData) = scalar * (*pVectorData);
                pOutputData += output.GetIncrement();
                pVectorData += vector.GetIncrement();
            }
        }

        // vectorB += scalarA * vectorA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB)
        {
            ElementType* pVectorBData = vectorB.GetDataPointer();
            const ElementType* pVectorAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                (*pVectorBData) += scalarA * (*pVectorAData);
                pVectorBData += vectorB.GetIncrement();
                pVectorAData += vectorA.GetIncrement();
            }
        }

        // vectorB =  scalarA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            ElementType* pVectorBData = vectorB.GetDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                (*pVectorBData) = scalarA + scalarB * (*pVectorBData);
                pVectorBData += vectorB.GetIncrement();
            }
        }

        // vectorB =  vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            ElementType* pVectorBData = vectorB.GetDataPointer();
            const ElementType* pVectorAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                (*pVectorBData) = (*pVectorAData) + scalarB * (*pVectorBData);
                pVectorBData += vectorB.GetIncrement();
                pVectorAData += vectorA.GetIncrement();
            }
        }

        // vectorB =  scalarA * vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            ElementType* pVectorBData = vectorB.GetDataPointer();
            const ElementType* pVectorAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                (*pVectorBData) = scalarA * (*pVectorAData) + scalarB * (*pVectorBData);
                pVectorBData += vectorB.GetIncrement();
                pVectorAData += vectorA.GetIncrement();
            }
        }

        // vectorC = scalarA * ones + scalarB * vectorB
        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            ElementType* pOutputData = output.GetDataPointer();
            const ElementType* pVectorBData = vectorB.GetConstDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                (*pOutputData) = scalarA + scalarB * (*pVectorBData);
                pVectorBData += vectorB.GetIncrement();
                pOutputData += output.GetIncrement();
            }
        }

        // vectorC = scalarA * vectorA + scalarB * vectorB
        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            ElementType* pOutputData = output.GetDataPointer();
            const ElementType* pVectortAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBData = vectorB.GetConstDataPointer();
            const ElementType* pOutputEnd = pOutputData + output.GetIncrement() * output.Size();

            while (pOutputData < pOutputEnd)
            {
                (*pOutputData) = scalarA * (*pVectortAData) + scalarB * (*pVectorBData);
                pVectortAData += vectorA.GetIncrement();
                pVectorBData += vectorB.GetIncrement();
                pOutputData += output.GetIncrement();
            }
        }

#ifdef USE_BLAS
        //
        // OpenBlasVectorOperations
        //

        template <typename ElementType>
        void VectorOperations<ImplementationType::openBlas>::InnerProduct(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result)
        {
            if (vectorA.Size() != vectorB.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectorA and vectorB are not the same size.");
            }

            result = Blas::Dot(static_cast<int>(vectorA.Size()), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetConstDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        template <typename ElementType, MatrixLayout layout>
        void VectorOperations<ImplementationType::openBlas>::OuterProduct(ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ConstVectorReference<ElementType, VectorOrientation::row> vectorB, MatrixReference<ElementType, layout> matrix)
        {
            if (vectorA.Size() != matrix.NumRows() || vectorB.Size() != matrix.NumColumns())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "inconsistent vector and matrix sizes");
            }

            Blas::Ger(matrix.GetLayout(), static_cast<int>(matrix.NumRows()), static_cast<int>(matrix.NumColumns()), static_cast<ElementType>(1.0), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetConstDataPointer(), static_cast<int>(vectorB.GetIncrement()), matrix.GetDataPointer(), static_cast<int>(matrix.GetIncrement()));
        }

        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            output.Fill(scalar);
            AddUpdate(vector, output);
        }

        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            output.CopyFrom(vectorA);
            AddUpdate(vectorB, output);
        }

        // vectorB += vectorA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
        {
            ScaleAddUpdate(static_cast<ElementType>(1.0), vectorA, One(), vectorB);
        }

        // vector *= scalar
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
        {
            Blas::Scal(static_cast<int>(vector.Size()), scalar, vector.GetDataPointer(), static_cast<int>(vector.GetIncrement()));
        }

        // output = scalar * vector
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            ScaleAddUpdate(scalar, vector, static_cast<ElementType>(0.0), output);
        }

        // vectorB += scalarA * vectorA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB)
        {
            Blas::Axpy(static_cast<int>(vectorB.Size()), scalarA, vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        // vectorB =  scalarA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            Blas::Scal(static_cast<int>(vectorB.Size()), scalarB, vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
            math::AddUpdate(scalarA, vectorB);
        }

        // vectorB =  vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            Blas::Scal(static_cast<int>(vectorB.Size()), scalarB, vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
            AddUpdate(vectorA, vectorB);
        }

        // vectorB = scalarA * vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            Blas::Scal(static_cast<int>(vectorB.Size()), scalarB, vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
            Blas::Axpy(static_cast<int>(vectorB.Size()), scalarA, vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        // vectorC = scalarA * ones + scalarB * vectorB
        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            output.Fill(scalarA);
            ScaleAddUpdate(scalarB, vectorB, One(), output);
        }

        // vectorC = scalarA * vectorA + scalarB * vectorB
        template<typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            ScaleSet(scalarA, vectorA, output);
            ScaleAddUpdate(scalarB, vectorB, One(), output);
        }

#endif // USE_BLAS
    }
}
}
