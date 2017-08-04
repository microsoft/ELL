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

    template <typename ElementType, VectorOrientation orientation>
    void Add(ElementType scalar, VectorReference<ElementType, orientation> vector)
    {
        vector.Transform([scalar](ElementType x) {return x + scalar; });
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator+=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        Add(static_cast<VectorElementType>(scalar), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator-=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        Add(-static_cast<VectorElementType>(scalar), vector);
    }

    template <typename ElementType, VectorOrientation orientation>
    void Add(ElementType scalar, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
    {
        Internal::VectorOperations<ImplementationType::openBlas>::Add(scalar, vectorA, vectorB);
    }

    template <typename ElementType, VectorOrientation orientation>
    void Multiply(ElementType scalar, VectorReference<ElementType, orientation> vector)
    {
        Internal::VectorOperations<ImplementationType::openBlas>::Multiply(scalar,vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator*=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        Multiply(static_cast<VectorElementType>(scalar), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator/=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        if (scalar == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero");
        }
        Multiply(1/static_cast<VectorElementType>(scalar), vector);
    }

    template <typename ElementType, VectorOrientation orientation>
    void MultiplyAdd(ElementType scalarA, ElementType scalarB, VectorReference<ElementType, orientation> vector)
    {
        if (scalarB == 0)
        {
            Multiply(scalarA, vector);
        }
        else
        {
            vector.Transform([scalarA, scalarB](ElementType x) { return (scalarA * x) + scalarB; });
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void ElementwiseMultiply(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB, VectorReference<ElementType, orientation> vectorC)
    {
        DEBUG_THROW(vectorA.Size() != vectorB.Size() || vectorA.Size() != vectorB.Size(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible vector sizes."));

        const ElementType* vectorAData = vectorA.GetDataPointer();
        const ElementType* vectorBData = vectorB.GetDataPointer();

        size_t i = 0;
        const ElementType* end = vectorA.GetDataPointer() + vectorA.GetIncrement() * vectorA.Size();

        while (vectorAData < end)
        {
            vectorC[i++] = (*vectorAData) * (*vectorBData);
            vectorAData += vectorA.GetIncrement();
            vectorBData += vectorB.GetIncrement();
        }
    }

    template <typename ElementType>
    ElementType Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB)
    {
        return Internal::VectorOperations<ImplementationType::openBlas>::Dot(vectorA, vectorB);
    }

    template <typename ElementType>
    void Multiply(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result)
    {
        result = Dot(vectorA, vectorB);
    }

    //
    // NativeVectorOperations
    //

    namespace Internal
    {
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::Add(ElementType scalar, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
        {
            if (vectorB.Size() != vectorA.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and vectorA are not the same size.");
            }

            ElementType* vectorBData = vectorB.GetDataPointer();
            const ElementType* vectorAData = vectorA.GetDataPointer();
            const ElementType* vectorBEnd = vectorB.GetDataPointer() + vectorB.GetIncrement() * vectorB.Size();

            while (vectorBData < vectorBEnd)
            {
                (*vectorBData) += scalar * (*vectorAData);
                vectorBData += vectorB.GetIncrement();
                vectorAData += vectorA.GetIncrement();
            }
        }

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::Multiply(ElementType value, VectorReference<ElementType, orientation> vector)
        {
            vector.Transform([value](ElementType x) {return x * value; });
        }

        template <typename ElementType>
        ElementType VectorOperations<ImplementationType::native>::Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB)
        {
            if (vectorA.Size() != vectorB.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectorA and vectorB are not the same size.");
            }

            const ElementType* vectorAData = vectorA.GetDataPointer();
            const ElementType* vectorBData = vectorB.GetDataPointer();

            ElementType result = 0;
            const ElementType* end = vectorA.GetDataPointer() + vectorA.GetIncrement() * vectorA.Size();

            while (vectorAData < end)
            {
                result += (*vectorAData) * (*vectorBData);
                vectorAData += vectorA.GetIncrement();
                vectorBData += vectorB.GetIncrement();
            }
            return result;
        }

#ifdef USE_BLAS
        //
        // OpenBlasVectorOperations
        //

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::Add(ElementType value, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
        {
            if (vectorA.Size() != vectorB.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors vectorB and vectorA are not the same size.");
            }

            Blas::Axpy(static_cast<int>(vectorB.Size()), value, vectorA.GetDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::Multiply(ElementType value, VectorReference<ElementType, orientation> vector)
        {
            Blas::Scal(static_cast<int>(vector.Size()), value, vector.GetDataPointer(), static_cast<int>(vector.GetIncrement()));
        }

        template <typename ElementType>
        ElementType VectorOperations<ImplementationType::openBlas>::Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB)
        {
            if (vectorA.Size() != vectorB.Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectorA and vectorB are not the same size.");
            }

            return Blas::Dot(static_cast<int>(vectorA.Size()), vectorA.GetDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

#endif // USE_BLAS
    }
}
}
