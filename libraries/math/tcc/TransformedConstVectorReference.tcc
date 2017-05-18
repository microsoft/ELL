////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TransformedConstVectorReference.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace math
{
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType>::TransformedConstVectorReference(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation) : _vector(vector), _transformation(std::move(transformation))
    {
    }

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType> TransformVector(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation)
    {
        return TransformedConstVectorReference<ElementType, orientation, TransformationType>(vector, transformation);
    }

    template <typename ElementType, VectorOrientation orientation>
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector)
    {
        ElementType typedScalar = static_cast<ElementType>(scalar);
        return TransformVector(vector, [typedScalar](ElementType x) { return typedScalar*x; });
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector)
    {
        return TransformVector(vector, [](ElementType x) { return x*x; });
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector)
    {
        return TransformVector(vector, [](ElementType x) { return std::sqrt(x); });
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector)
    {
        return TransformVector(vector, [](ElementType x) { return std::abs(x); });
    }
}
}