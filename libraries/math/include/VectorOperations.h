////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BlasWrapper.h"
#include "Common.h"
#include "Vector.h"
#include "Matrix.h"

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

    /// \name Scalar operators
    /// @{

    /// <summary> Adds a scalar to a vector. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="vector"> The vector to which the scalar is added. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator+=(VectorReference<VectorElementType, orientation> vector, ScalarType scalar);

    /// <summary> Subtracts a scalar from a vector. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="vector"> The vector from which the scalar is subtracted. </param>
    /// <param name="scalar"> The constant value. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator-=(VectorReference<VectorElementType, orientation> vector, ScalarType scalar);

    /// <summary> Multiplies this vector by a constant value. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="vector"> The vector being scaled. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator*=(VectorReference<VectorElementType, orientation> vector, ScalarType scalar);

    /// <summary> Divides each element of this vector by a non-zero scalar. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="vector"> The vector being divided. </param>
    /// <param name="scalar"> The non-zero scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator/=(VectorReference<VectorElementType, orientation> vector, ScalarType scalar);

    /// @}

    /// \name Vector multiply
    /// @{

    /// <summary> Vector vector element wise multiplication, lhsVector = rhsvectorA .* rhsvectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientaton. </typeparam>
    /// <param name="vectorA"> The first right hand side vector. </param>
    /// <param name="vectorB"> The second right hand side vector. </param>
    /// <param name="output"> The left hand side vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void ElementwiseMultiplySet(UnorientedConstVectorReference<ElementType> vectorA, UnorientedConstVectorReference<ElementType> vectorB, VectorReference<ElementType, orientation> output);

    /// <summary> Calculates the product of a row vector with a column vector, result = vectorA * vectorB. Same as Dot. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <param name="vectorA"> The left vector in row orientation. </param>
    /// <param name="vectorB"> The right vector in column orientation. </param>
    /// <param name="result"> [out] The scalar used to store the result. </param>
    template <typename ElementType>
    void InnerProduct(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result);

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

    /// <summary> Calculates the outer product of a column vector with a row vector, resulting in a matrix result = vectorA * vectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <param name="vectorA"> The left vector in column orientation. </param>
    /// <param name="vectorB"> The right vector in row orientation. </param>
    /// <param name="result"> [out] The matrix used to store the result. </param>
    template <typename ElementType, MatrixLayout layout>
    void OuterProduct(ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ConstVectorReference<ElementType, VectorOrientation::row> vectorB, MatrixReference<ElementType, layout> matrix);

    /// @}

    /// \name Addition
    /// @{

    /// <summary> Performs the linear operation vector += scalar. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector, which is updated. </param>
    template <typename ElementType, VectorOrientation orientation>
    void AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

    /// <summary> Performs the linear operation vectorB += vectorA. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vectorA"> The first vector. </param>
    /// <param name="vectorB"> The second vector, which is updated. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

    /// <summary> Performs the linear operation output = scalar * ones + vector. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector. </param>
    /// <param name="output"> The output vector. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

    /// <summary> Performs the linear operation output = vectorA + vectorB. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vectorA"> The first vector. </param>
    /// <param name="vectorB"> The second vector. </param>
    /// <param name="output"> The output vector. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    /// @}

    /// \name Scaling
    /// @{

    /// <summary> Performs the linear operation vector *= scalar </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector, which is updated. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

    /// <summary> Performs the linear operation output = scalarA * vectorA. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector. </param>
    /// <param name="output"> The output. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

    /// @}

    /// @{
    /// <summary>
    /// Performs the linear operation vectorB = scalarA * vectorA + scalarB * vectorB.
    /// </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first vector: either a real number or One().</param>
    /// <param name="vectorA"> The first vector or OnesVector(). </param>
    /// <param name="scalarB"> The scalar that multiplies the second vector: either a real number or One() </param>
    /// <param name="vectorB"> The second vector, which is updated. </param>

    // vectorB += scalarA * vectorA
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB);

    // vectorB = scalarA * ones + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

    // vectorB = vectorA + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

    // vectorB = scalarA * vectorA + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

    /// @}

    /// @{
    /// <summary>
    /// Performs the linear operation output = scalarA * vectorA + scalarB * vectorB.
    /// </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalarA"> The scalar that multiplies the first vector.</param>
    /// <param name="vectorA"> The first vector or OnesVector(). </param>
    /// <param name="scalarB"> The scalar that multiplies the second vector.</param>
    /// <param name="vectorB"> The second vector. </param>
    /// <param name="output"> The output vector. </param>

    // vectorC = scalarA * ones + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    // vectorC = scalarA * vectorA + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    /// @}
    
    /// \name Cumulative sum (integral) and consecutive difference (derivative)
    /// @{

    /// <summary> Replaces a vector with its cumulative sum (vector integral). </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void CumulativeSumUpdate(VectorReference<ElementType, orientation> vector);

    /// <summary> Replaces a vector with the difference between consecutive elements (vector derivative). </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void ConsecutiveDifferenceUpdate(VectorReference<ElementType, orientation> vector);

    /// @}

    namespace Internal
    {
        template <ImplementationType implementation>
        struct VectorOperations
        {};

        template<>
        struct VectorOperations<ImplementationType::native>
        {
            static std::string GetImplementationName() { return "Native"; }

            template <typename ElementType>
            static void InnerProduct(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result);

            template <typename ElementType, MatrixLayout layout>
            static void OuterProduct(ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ConstVectorReference<ElementType, VectorOrientation::row> vectorB, MatrixReference<ElementType, layout> matrix);

            // vectorB += vectorA
            template <typename ElementType, VectorOrientation orientation>
            static void AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

            // output = scalar + vector
            template <typename ElementType, VectorOrientation orientation>
            static void AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

            // output = vectorA + vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vector *= scalar
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

            // output = scalar * vector
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

            // vectorB = scalarA * vectorA
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB);

            // vectorB = scalarA * ones + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

            // vectorB = vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

            // vectorB = scalarA * vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

            // vectorC = scalarA * ones + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorC = scalarA * vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);
        };

#ifdef USE_BLAS
        template<>
        struct VectorOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBlas"; }

            template <typename ElementType>
            static void InnerProduct(ConstVectorReference<ElementType, VectorOrientation::row> vectorA, ConstVectorReference<ElementType, VectorOrientation::column> vectorB, ElementType& result);

            template <typename ElementType, MatrixLayout layout>
            static void OuterProduct(ConstVectorReference<ElementType, VectorOrientation::column> vectorA, ConstVectorReference<ElementType, VectorOrientation::row> vectorB, MatrixReference<ElementType, layout> matrix);

            // vectorB += vectorA
            template <typename ElementType, VectorOrientation orientation>
            static void AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

            // output = scalar + vector
            template <typename ElementType, VectorOrientation orientation>
            static void AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

            // output = vectorA + vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vector *= scalar
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

            // output = scalar * vector
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

            // vectorB = scalarA * vectorA
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB);

            // vectorB = scalarA * ones + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

            // vectorB = vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

            // vectorB = scalarA * vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB);

            // vectorC = scalarA * ones + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorC = scalarA * vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);
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
