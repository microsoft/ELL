////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.h"
#include "Vector.h"
#include "BlasWrapper.h"

// utilities
#include "TypeTraits.h"

// stl
#include <string>
#include <ostream>

namespace ell
{
namespace math
{
    /// \name Utility Functions
    /// @{

    /// <summary> Prints a vector in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="indent"> (Optional) How many tabs to print before the tensor. </param>
    /// <param name="maxElements"> (Optional) The maximal number of elements to print. </param>
    template <typename ElementType, VectorOrientation orientation>
    void Print(ConstVectorReference<ElementType, orientation> vector, std::ostream& stream, size_t indent = 0, size_t maxElements = std::numeric_limits<size_t>::max());

    /// <summary> Prints a vector in initializer list format. </summary>
    ///
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="vector"> The const vector reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, VectorOrientation orientation>
    std::ostream& operator<<(std::ostream& stream, ConstVectorReference<ElementType, orientation> vector);

    /// @}

    /// \name TransformedVector Generators
    /// @{

    /// <summary> Transformations that apply to individual elements in the vector. </summary>
    template<typename ElementType>
    using ElementTransformation = ElementType(*)(ElementType);

    /// <summary> Elementwise square operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ElementTransformation<ElementType>>;

    /// <summary> Elementwise square-root operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square-root operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ElementTransformation<ElementType>>;

    /// <summary> Elementwise absolute value operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise absolute value operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ElementTransformation<ElementType>>;

    /// <summary> A helper class for scalar vector multiplication. </summary>
    ///
    /// <typeparam name="ElementType"> Type of the element type. </typeparam>
    template <typename ElementType>
    struct ScaleFunction
    {
        ElementType _value;
        ElementType operator()(ElementType x);
    };

    /// <summary> Multiplication operator for scalar and vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference that results from the operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector)->TransformedConstVectorReference<ElementType, orientation, ScaleFunction<ElementType>>;

    /// @}

    /// \name Addition
    /// @{

    /// <summary> Adds a scalar to a vector. </summary>
    /// \anchor VectorScalarAdd
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector vector orientation. </typeparam>
    /// <param name="scalar"> The scalar being added. </param>
    /// <param name="vector"> The vector to which the scalar is added. </param>
    template <typename ElementType, VectorOrientation orientation>
    void Add(ElementType scalar, VectorReference<ElementType, orientation> vector);

    /// <summary> Adds a scalar to a vector. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="vector"> The vector to which the scalar is added. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator+=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar);

    /// <summary> Subtracts a scalar from a vector. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="vector"> The vector from which the scalar is subtracted. </param>
    /// <param name="scalar"> The constant value. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator-=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar);

    /// <summary> Adds a scaled vector to another vector, lhsVector += value * rhsVector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Orientation of the two vectors. </typeparam>
    /// <param name="scalar"> The scalar that multiplies the right-hand sidevector. </param>
    /// <param name="vectorA"> The right-hand side vector. </param>
    /// <param name="vectorB"> The left-hand side vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void Add(ElementType scalar, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

    /// @}

    /// \name Multiplication
    /// @{

    /// <summary> Multiplies a vector by a scalar. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector being scaled. </param>
    template <typename ElementType, VectorOrientation orientation>
    void Multiply(ElementType scalar, VectorReference<ElementType, orientation> vector);

    /// <summary> Multiplies this vector by a constant value. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="vector"> The vector being scaled. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator*=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar);

    /// <summary> Divides each element of this vector by a non-zero scalar. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarElementType"> Scalar element type. </typeparam>
    /// <param name="vector"> The vector being divided. </param>
    /// <param name="scalar"> The non-zero scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept = true>
    void operator/=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar);

    /// <summary> Multiplies a vector by scalarA and then add scalarB to it, vector = scalarA * vector + scalarB. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the vector. </param>
    /// <param name="scalarB"> The scalar added to the vector. </param>
    /// <param name="vector"> The vector being modified. </param>
    template <typename ElementType, VectorOrientation orientation>
    void MultiplyAdd(ElementType scalarA, ElementType scalarB, VectorReference<ElementType, orientation> vector);

    /// <summary> Vector vector element wise multiplication, lhsVector = rhsvectorA .* rhsvectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientaton. </typeparam>
    /// <param name="vectorA"> The first right hand side vector. </param>
    /// <param name="vectorB"> The second right hand side vector. </param>
    /// <param name="vectorC"> The left hand side vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void ElementwiseMultiply(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB, VectorReference<ElementType, orientation> vectorC);

    /// <summary>
    /// Calculates a vector dot product (between vectors in any orientation), return vectorA * vectorB
    /// </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <param name="vectorA"> The first vector, in any orientation. </param>
    /// <param name="vectorB"> The second vector, in any orientation. </param>
    ///
    /// <returns> The dot Multiply. </returns>
    template <typename ElementType>
    ElementType Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB);

    /// <summary> Calculates the product of a row vector with a column vector, result = vectorA * vectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <param name="vectorA"> The left vector in row orientation. </param>
    /// <param name="vectorB"> The right vector in column orientation. </param>
    /// <param name="result"> [out] The scalar used to store the result. </param>
    template <typename ElementType>
    void Multiply(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result);

    /// @}

    namespace Internal
    {
        template <ImplementationType type>
        struct VectorOperations
        {};

        template<>
        struct VectorOperations<ImplementationType::native>
        {
            static std::string GetImplementationName() { return "Native"; }

            template <typename ElementType, VectorOrientation orientation>
            static void Add(ElementType scalar, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

            template <typename ElementType, VectorOrientation orientation>
            static void Multiply(ElementType scalar, VectorReference<ElementType, orientation> vector);

            template <typename ElementType>
            static ElementType Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB);
        };

#ifdef USE_BLAS
        template<>
        struct VectorOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBlas"; }

            template <typename ElementType, VectorOrientation orientation>
            static void Add(ElementType scalar, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

            template <typename ElementType, VectorOrientation orientation>
            static void Multiply(ElementType scalar, VectorReference<ElementType, orientation> vector);

            template <typename ElementType>
            static ElementType Dot(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB);
        };

#else
        template<>
        struct VectorOperations<ImplementationType::openBlas> : public VectorOperations<ImplementationType::native>
        {};

#endif // USE_BLAS
    }
}
}

#include "../tcc/VectorOperations.tcc"