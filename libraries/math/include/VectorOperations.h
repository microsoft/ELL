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
#include "Matrix.h"
#include "Transformations.h"
#include "Vector.h"

#include <utilities/include/TypeTraits.h>

#include <ostream>
#include <string>

namespace ell
{
namespace math
{
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

    /// <summary> Returns the transpose vector. </summary>
    ///
    /// <param name="vector"> The const vector reference to transpose. </param>
    ///
    /// <returns> The transpose. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Transpose(ConstVectorReference<ElementType, orientation> vector)
    {
        return vector.Transpose();
    }

    /// <summary> A class that represents a transformed constant vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    class TransformedConstVectorReference
    {
    public:
        /// <summary> Constructs an instance of TransformedConstVectorReference. </summary>
        ///
        /// <param name="vector"> The vector. </param>
        /// <param name="transform"> The transformation. </param>
        TransformedConstVectorReference(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation);

        /// <summary> Gets the transformation. </summary>
        ///
        /// <returns> The transformation. </returns>
        const TransformationType GetTransformation() { return _transformation; }

        /// <summary> Gets the vector reference. </summary>
        ///
        /// <returns> The vector reference. </returns>
        ConstVectorReference<ElementType, orientation> GetVector() const { return _vector; }

    private:
        ConstVectorReference<ElementType, orientation> _vector;
        TransformationType _transformation;
    };

    /// <summary> Helper function that constructs a TransformedConstVectorReference from a vector and a transformation. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    /// <typeparam name="TransformationType"> The transformation type. </typeparam>
    /// <param name="vector"> The vector. </param>
    /// <param name="transformation"> The transformation. </param>
    ///
    /// <returns> A TransformedConstVectorReference. </returns>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType> TransformVector(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation);

    /// <summary> Elementwise square operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, Transformation<ElementType>>;

    /// <summary> Elementwise square-root operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise square-root operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, Transformation<ElementType>>;

    /// <summary> Elementwise absolute value operation on a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> The TransformedConstVectorReference generated from the vector and a elementwise absolute value operation. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, Transformation<ElementType>>;

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
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, ScaleFunction<ElementType>>;

    /// <summary> Returns the transpose vector. </summary>
    ///
    /// <param name="vector"> The vector reference to transpose. </param>
    ///
    /// <returns> The transpose. </returns>
    template <typename ElementType, VectorOrientation orientation>
    auto Transpose(VectorReference<ElementType, orientation> vector)
    {
        return vector.Transpose();
    }

    /// <summary> Adds a scalar to a vector. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="vector"> The vector to which the scalar is added. </param>
    /// <param name="scalar"> The scalar. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator+=(VectorReference<VectorElementType, orientation> vector, ScalarType scalar);

    /// <summary> Adds a vector to a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <param name="vectorB"> The lhs vector, the one being modified. </param>
    /// <param name="vectorA"> The rhs vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void operator+=(VectorReference<ElementType, orientation> vectorB, ConstVectorReference<ElementType, orientation> vectorA);

    /// <summary> Adds a transformed vector to a vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <param name="vector"> The lhs vector, the one being modified. </param>
    /// <param name="transformedVector"> The rhs transformed vector. </param>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void operator+=(VectorReference<ElementType, orientation> vector, TransformedConstVectorReference<ElementType, orientation, TransformationType> transformedVector);

    /// <summary> Subtracts a scalar from a vector. </summary>
    ///
    /// <typeparam name="VectorElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <typeparam name="ScalarType"> Scalar type. </typeparam>
    /// <param name="vector"> The vector from which the scalar is subtracted. </param>
    /// <param name="scalar"> The constant value. </param>
    template <typename VectorElementType, VectorOrientation orientation, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator-=(VectorReference<VectorElementType, orientation> vector, ScalarType scalar);

    /// <summary> Subtracts a vector from another vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> Vector orientation. </typeparam>
    /// <param name="vectorB"> The lhs vector, the one being modified. </param>
    /// <param name="vectorA"> The rhs vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void operator-=(VectorReference<ElementType, orientation> vectorB, ConstVectorReference<ElementType, orientation> vectorA);

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

    /// <summary> Performs the linear operation vector += scalar. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector, which is updated. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
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

    /// <summary> Multiplies a vector by a scalar in place, vector *= scalar </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector, which is updated. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

    /// <summary> Multiplies a vector by a scalar and stores the result in an output vector, output = scalarA * vectorA. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="scalar"> The scalar. </param>
    /// <param name="vector"> The vector. </param>
    /// <param name="output"> The output. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

    /// <summary> A stub class that represents the all-ones vector. </summary>
    struct OnesVector
    {};

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

    // output = scalarA * vectorA + vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    // output = scalarA * ones + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    // output = vectorA + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    // output = scalarA * vectorA + scalarB * vectorB
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);
    /// @}

    /// <summary> Vector vector element wise multiplication, lhsVector = rhsvectorA .* rhsvectorB. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientaton. </typeparam>
    /// <param name="vectorA"> The first right hand side vector. </param>
    /// <param name="vectorB"> The second right hand side vector. </param>
    /// <param name="output"> The left hand side vector. </param>
    template <typename ElementType, VectorOrientation orientation>
    void ElementwiseMultiplySet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

    /// <summary> Calculates the product of a row vector with a column vector, result = vectorA * vectorB. Same as Dot. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <param name="vectorA"> The left vector in row orientation. </param>
    /// <param name="vectorB"> The right vector in column orientation. </param>
    /// <param name="result"> [out] The scalar used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType>
    void InnerProduct(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB, ElementType& result);

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
    ElementType Dot(UnorientedConstVectorBase<ElementType> vectorA, UnorientedConstVectorBase<ElementType> vectorB);

    /// <summary> Calculates the outer product of a column vector with a row vector, resulting in a matrix result = vectorA * vectorB. </summary>
    ///
    /// <typeparam name="implementation"> The implementation. </typeparam>
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <param name="vectorA"> The left vector in column orientation. </param>
    /// <param name="vectorB"> The right vector in row orientation. </param>
    /// <param name="result"> [out] The matrix used to store the result. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, MatrixLayout layout>
    void OuterProduct(ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix);

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

    /// <summary> Applies an elementwise transformation in place. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="transformation"> The elementwise transformation. </param>
    /// <param name="vector"> The vector. </param>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void TransformUpdate(TransformationType transformation, VectorReference<ElementType, orientation> vector);

    /// <summary> Applies an elementwise transformation to a vector and places the result in another vector. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="transformation"> The elementwise transformation. </param>
    /// <param name="vector"> The input vector, whose elements are the input of the transformation. </param>
    /// <param name="output"> The output vector. </param>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void TransformSet(TransformationType transformation, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output);

    /// <summary> Applies an elementwise transformation to a vector and adds the result to another vector. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="orientation"> The vector orientation. </typeparam>
    /// <param name="transformation"> The elementwise transformation. </param>
    /// <param name="vectorA"> The input vector, whose elements are the input of the transformation. </param>
    /// <param name="vectorB"> The output vector. </param>
    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void TransformAddUpdate(TransformationType transformation, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB);

    namespace Internal
    {
        template <ImplementationType implementation>
        struct VectorOperations
        {};

        template <>
        struct VectorOperations<ImplementationType::native>
        {
            static std::string GetImplementationName() { return "Native"; }

            template <typename ElementType>
            static void InnerProduct(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB, ElementType& result);

            template <typename ElementType, MatrixLayout layout>
            static void OuterProduct(ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix);

            // vector += scalar
            template <typename ElementType, VectorOrientation orientation>
            static void AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

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

            // output = scalarA * vectorA + vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorC = scalarA * ones + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorB = vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorC = scalarA * vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);
        };

#ifdef USE_BLAS
        template <>
        struct VectorOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBLAS"; }

            template <typename ElementType>
            static void InnerProduct(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB, ElementType& result);

            template <typename ElementType, MatrixLayout layout>
            static void OuterProduct(ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix);

            // vector += scalar
            template <typename ElementType, VectorOrientation orientation>
            static void AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector);

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

            // output = scalarA * vectorA + vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorC = scalarA * ones + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorB = vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);

            // vectorC = scalarA * vectorA + scalarB * vectorB
            template <typename ElementType, VectorOrientation orientation>
            static void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output);
        };

#else
        template <>
        struct VectorOperations<ImplementationType::openBlas> : public VectorOperations<ImplementationType::native>
        {};

#endif // USE_BLAS
    } // namespace Internal
} // namespace math
} // namespace ell

#pragma region implementation

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>

namespace ell
{
namespace math
{
    template <typename ElementType, VectorOrientation orientation>
    void Print(ConstVectorReference<ElementType, orientation> vector, std::ostream& stream, size_t indent, size_t maxElements)
    {
        DEBUG_CHECK_SIZES(maxElements < 3, "cannot specify maxElements below 3.");

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

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType>::TransformedConstVectorReference(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation) :
        _vector(vector),
        _transformation(std::move(transformation))
    {
    }

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    TransformedConstVectorReference<ElementType, orientation, TransformationType> TransformVector(ConstVectorReference<ElementType, orientation> vector, TransformationType transformation)
    {
        return TransformedConstVectorReference<ElementType, orientation, TransformationType>(vector, transformation);
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Square(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, Transformation<ElementType>>
    {
        return TransformVector(vector, SquareTransformation<ElementType>);
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Sqrt(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, Transformation<ElementType>>
    {
        return TransformVector(vector, SquareRootTransformation<ElementType>);
    }

    template <typename ElementType, VectorOrientation orientation>
    auto Abs(ConstVectorReference<ElementType, orientation> vector) -> TransformedConstVectorReference<ElementType, orientation, Transformation<ElementType>>
    {
        return TransformVector(vector, AbsoluteValueTransformation<ElementType>);
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

    template <typename ElementType, VectorOrientation orientation>
    void operator+=(VectorReference<ElementType, orientation> vectorB, ConstVectorReference<ElementType, orientation> vectorA)
    {
        AddUpdate(vectorA, vectorB);
    }

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void operator+=(VectorReference<ElementType, orientation> vector, TransformedConstVectorReference<ElementType, orientation, TransformationType> transformedVector)
    {
        TransformAddUpdate(transformedVector.GetTransformation(), transformedVector.GetVector(), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator-=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        AddUpdate(static_cast<VectorElementType>(-scalar), vector);
    }

    template <typename ElementType, VectorOrientation orientation>
    void operator-=(VectorReference<ElementType, orientation> vectorB, ConstVectorReference<ElementType, orientation> vectorA)
    {
        ScaleAddUpdate(static_cast<ElementType>(-1), vectorA, One(), vectorB);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator*=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        ScaleUpdate(static_cast<VectorElementType>(scalar), vector);
    }

    template <typename VectorElementType, VectorOrientation orientation, typename ScalarElementType, utilities::IsFundamental<ScalarElementType> concept>
    void operator/=(VectorReference<VectorElementType, orientation> vector, ScalarElementType scalar)
    {
        DEBUG_THROW(scalar == 0, utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "Divide by zero."));

        ScaleUpdate(1 / static_cast<VectorElementType>(scalar), vector);
    }

    // vector += scalar
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
    {
        if (scalar == 0)
        {
            return;
        }
        else
        {
            Internal::VectorOperations<implementation>::AddUpdate(scalar, vector);
        }
    }

    // vectorB += vectorA
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
    {
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size(), "Incompatible vector sizes.");

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
        DEBUG_CHECK_SIZES(vectorA.Size() != vectorB.Size(), "Incompatible vector sizes.");

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
        DEBUG_CHECK_SIZES(vector.Size() != output.Size(), "Incompatible vector sizes.");

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
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size(), "Incompatible vector sizes.");

        if (scalarA == 0)
        {
            return;
        }
        else if (scalarA == 1)
        {
            AddUpdate<implementation>(vectorA, vectorB);
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
        else if (scalarB == 1)
        {
            Internal::VectorOperations<implementation>::AddUpdate(scalarA, vectorB);
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
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size(), "Incompatible vector sizes.");

        if (scalarB == 0)
        {
            vectorB.CopyFrom(vectorA);
        }
        else if (scalarB == 1)
        {
            Internal::VectorOperations<implementation>::AddUpdate(vectorA, vectorB);
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
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size(), "Incompatible vector sizes.");

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
            Internal::VectorOperations<implementation>::ScaleSet(scalarA, vectorA, vectorB);
        }
        else if (scalarB == 1)
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, vectorA, One(), vectorB);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddUpdate(scalarA, vectorA, scalarB, vectorB);
        }
    }

    // output = scalarA * vectorA + vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size() || vectorA.Size() != output.Size(), "Incompatible vector sizes.");

        if (scalarA == 0)
        {
            output.CopyFrom(vectorB);
        }
        else if (scalarA == 1)
        {
            Internal::VectorOperations<implementation>::AddSet(vectorA, vectorB, output);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, vectorA, One(), vectorB, output);
        }
    }

    // output = scalarA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        DEBUG_CHECK_SIZES(vectorB.Size() != output.Size(), "Incompatible vector sizes.");

        if (scalarA == 0)
        {
            ScaleSet<implementation>(scalarB, vectorB, output);
        }
        else if (scalarB == 0)
        {
            output.Fill(scalarA);
        }
        else if (scalarB == 1)
        {
            Internal::VectorOperations<implementation>::AddSet(scalarA, vectorB, output);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, OnesVector(), scalarB, vectorB, output);
        }
    }

    // output = vectorA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size() || vectorA.Size() != output.Size(), "Incompatible vector sizes.");

        if (scalarB == 0)
        {
            output.CopyFrom(vectorA);
        }
        else if (scalarB == 1)
        {
            Internal::VectorOperations<implementation>::AddSet(vectorA, vectorB, output);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(One(), vectorA, scalarB, vectorB, output);
        }
    }

    // output = scalarA * vectorA + scalarB * vectorB
    template <ImplementationType implementation, typename ElementType, VectorOrientation orientation>
    void ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
    {
        DEBUG_CHECK_SIZES(vectorB.Size() != vectorA.Size() || vectorB.Size() != output.Size(), "Incompatible vector sizes.");

        if (scalarA == 0)
        {
            ScaleSet<implementation>(scalarB, vectorB, output);
        }
        else if (scalarA == 1)
        {
            ScaleAddSet<implementation>(One(), vectorA, scalarB, vectorB, output);
        }
        else if (scalarB == 0)
        {
            Internal::VectorOperations<implementation>::ScaleSet(scalarA, vectorA, output);
        }
        else if (scalarB == 1)
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, vectorA, One(), vectorB, output);
        }
        else
        {
            Internal::VectorOperations<implementation>::ScaleAddSet(scalarA, vectorA, scalarB, vectorB, output);
        }
    }

    template <typename ElementType, VectorOrientation orientation>
    void ElementwiseMultiplySet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> vectorC)
    {
        DEBUG_CHECK_SIZES(vectorA.Size() != vectorB.Size() || vectorA.Size() != vectorB.Size(), "Incompatible vector sizes.");

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

    template <ImplementationType implementation, typename ElementType>
    void InnerProduct(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB, ElementType& result)
    {
        DEBUG_CHECK_SIZES(vectorA.Size() != vectorB.Size(), "Incompatible vector sizes.");

        Internal::VectorOperations<implementation>::InnerProduct(vectorA, vectorB, result);
    }

    template <typename ElementType>
    ElementType operator*(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB)
    {
        ElementType result;
        InnerProduct(vectorA, vectorB, result);
        return result;
    }

    template <typename ElementType>
    ElementType Dot(UnorientedConstVectorBase<ElementType> vectorA, UnorientedConstVectorBase<ElementType> vectorB)
    {
        ConstRowVectorReference<ElementType> rowVector(vectorA.GetConstDataPointer(), vectorA.Size(), vectorA.GetIncrement());
        ConstColumnVectorReference<ElementType> columnVector(vectorB.GetConstDataPointer(), vectorB.Size(), vectorB.GetIncrement());

        ElementType result;
        InnerProduct(rowVector, columnVector, result);
        return result;
    }

    template <ImplementationType implementation, typename ElementType, MatrixLayout layout>
    void OuterProduct(ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix)
    {
        DEBUG_CHECK_SIZES(vectorA.Size() != matrix.NumRows() || vectorB.Size() != matrix.NumColumns(), "Incompatible vector matrix sizes.");

        Internal::VectorOperations<implementation>::OuterProduct(vectorA, vectorB, matrix);
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

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void TransformUpdate(TransformationType transformation, VectorReference<ElementType, orientation> vector)
    {
        vector.Transform(transformation);
    }

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void TransformSet(TransformationType transformation, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
    {
        DEBUG_CHECK_SIZES(vector.Size() != output.Size(), "Incompatible vector sizes.");

        ElementType* pOutputData = output.GetDataPointer();
        const ElementType* pVectorData = vector.GetConstDataPointer();
        const ElementType* pOutputEnd = pOutputData + output.Size() * output.GetIncrement();
        while (pOutputData < pOutputEnd)
        {
            *pOutputData = transformation(*pVectorData);
            pOutputData += output.GetIncrement();
            pVectorData += vector.GetIncrement();
        }
    }

    template <typename ElementType, VectorOrientation orientation, typename TransformationType>
    void TransformAddUpdate(TransformationType transformation, ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
    {
        DEBUG_CHECK_SIZES(vectorA.Size() != vectorB.Size(), "Incompatible vector sizes.");

        ElementType* pVectorBData = vectorB.GetDataPointer();
        const ElementType* pVectorAData = vectorA.GetConstDataPointer();
        const ElementType* pVectorBEnd = pVectorBData + vectorB.Size() * vectorB.GetIncrement();
        while (pVectorBData < pVectorBEnd)
        {
            *pVectorBData += transformation(*pVectorAData);
            pVectorBData += vectorB.GetIncrement();
            pVectorAData += vectorA.GetIncrement();
        }
    }

    //
    // NativeVectorOperations
    //
    namespace Internal
    {
        template <typename ElementType>
        void VectorOperations<ImplementationType::native>::InnerProduct(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB, ElementType& result)
        {
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
        void VectorOperations<ImplementationType::native>::OuterProduct(ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix)
        {
            for (size_t i = 0; i < matrix.NumRows(); ++i)
            {
                for (size_t j = 0; j < matrix.NumColumns(); ++j)
                {
                    matrix(i, j) = vectorA[i] * vectorB[j];
                }
            }
        }

        template <typename ElementType, VectorOrientation orientation, typename BinaryOperation>
        void UnaryVectorUpdateImplementation(VectorReference<ElementType, orientation> vector, BinaryOperation unaryOperation)
        {
            ElementType* pData = vector.GetDataPointer();
            const ElementType* pEnd = pData + vector.GetIncrement() * vector.Size();

            while (pData < pEnd)
            {
                unaryOperation(*pData);
                pData += vector.GetIncrement();
            }
        }

        template <typename ElementType, VectorOrientation orientation, typename BinaryOperation>
        void BinaryVectorUpdateImplementation(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB, BinaryOperation binaryOperation)
        {
            ElementType* pVectorBData = vectorB.GetDataPointer();
            const ElementType* pVectorAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBEnd = pVectorBData + vectorB.GetIncrement() * vectorB.Size();

            while (pVectorBData < pVectorBEnd)
            {
                binaryOperation(*pVectorAData, *pVectorBData);
                pVectorBData += vectorB.GetIncrement();
                pVectorAData += vectorA.GetIncrement();
            }
        }

        template <typename ElementType, VectorOrientation orientation, typename TrinaryOperation>
        void TrinaryVectorUpdateImplementation(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output, TrinaryOperation trinaryOperation)
        {
            ElementType* pOutputData = output.GetDataPointer();
            const ElementType* pVectortAData = vectorA.GetConstDataPointer();
            const ElementType* pVectorBData = vectorB.GetConstDataPointer();
            const ElementType* pOutputEnd = pOutputData + output.GetIncrement() * output.Size();

            while (pOutputData < pOutputEnd)
            {
                trinaryOperation(*pVectortAData, *pVectorBData, *pOutputData);
                pVectortAData += vectorA.GetIncrement();
                pVectorBData += vectorB.GetIncrement();
                pOutputData += output.GetIncrement();
            }
        }

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
        {
            UnaryVectorUpdateImplementation(vector, [scalar](ElementType& v) { v += scalar; });
        }

        // vectorB += scalarA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddUpdate(ConstVectorReference<ElementType, orientation> vectorA, VectorReference<ElementType, orientation> vectorB)
        {
            BinaryVectorUpdateImplementation(vectorA, vectorB, [](ElementType a, ElementType& b) { b += a; });
        }

        // output = scalar + vector
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            BinaryVectorUpdateImplementation(vector, output, [scalar](ElementType a, ElementType& o) { o = scalar + a; });
        }

        // output = vectorA + vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            TrinaryVectorUpdateImplementation(vectorA, vectorB, output, [](ElementType a, ElementType b, ElementType& o) { o = a + b; });
        }

        // vector *= scalar
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
        {
            UnaryVectorUpdateImplementation(vector, [scalar](ElementType& v) { v *= scalar; });
        }

        // output = scalar * vector
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            BinaryVectorUpdateImplementation(vector, output, [scalar](ElementType a, ElementType& o) { o = scalar * a; });
        }

        // vectorB += scalarA * vectorA
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, VectorReference<ElementType, orientation> vectorB)
        {
            BinaryVectorUpdateImplementation(vectorA, vectorB, [scalarA](ElementType a, ElementType& b) { b += scalarA * a; });
        }

        // vectorB =  scalarA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(ElementType scalarA, OnesVector, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            UnaryVectorUpdateImplementation(vectorB, [scalarA, scalarB](ElementType& b) { b = scalarA + scalarB * b; });
        }

        // vectorB =  vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            BinaryVectorUpdateImplementation(vectorA, vectorB, [scalarB](ElementType a, ElementType& b) { b = a + scalarB * b; });
        }

        // vectorB =  scalarA * vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddUpdate(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, VectorReference<ElementType, orientation> vectorB)
        {
            BinaryVectorUpdateImplementation(vectorA, vectorB, [scalarA, scalarB](ElementType a, ElementType& b) { b = scalarA * a + scalarB * b; });
        }

        // output = scalarA * vectorA + vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            TrinaryVectorUpdateImplementation(vectorA, vectorB, output, [scalarA](ElementType a, ElementType b, ElementType& o) { o = scalarA * a + b; });
        }

        // output = scalarA * ones + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            BinaryVectorUpdateImplementation(vectorB, output, [scalarA, scalarB](ElementType b, ElementType& o) { o = scalarA + scalarB * b; });
        }

        // vectorB = vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddSet(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            TrinaryVectorUpdateImplementation(vectorA, vectorB, output, [scalarB](ElementType a, ElementType b, ElementType& o) { o = a + scalarB * b; });
        }

        // output = scalarA * vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::native>::ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            TrinaryVectorUpdateImplementation(vectorA, vectorB, output, [scalarA, scalarB](ElementType a, ElementType b, ElementType& o) { o = scalarA * a + scalarB * b; });
        }

#ifdef USE_BLAS
        //
        // OpenBlasVectorOperations
        //

        template <typename ElementType>
        void VectorOperations<ImplementationType::openBlas>::InnerProduct(ConstRowVectorReference<ElementType> vectorA, ConstColumnVectorReference<ElementType> vectorB, ElementType& result)
        {
            result = Blas::Dot(static_cast<int>(vectorA.Size()), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetConstDataPointer(), static_cast<int>(vectorB.GetIncrement()));
        }

        template <typename ElementType, MatrixLayout layout>
        void VectorOperations<ImplementationType::openBlas>::OuterProduct(ConstColumnVectorReference<ElementType> vectorA, ConstRowVectorReference<ElementType> vectorB, MatrixReference<ElementType, layout> matrix)
        {
            matrix.Reset();
            Blas::Ger(matrix.GetLayout(), static_cast<int>(matrix.NumRows()), static_cast<int>(matrix.NumColumns()), static_cast<ElementType>(1.0), vectorA.GetConstDataPointer(), static_cast<int>(vectorA.GetIncrement()), vectorB.GetConstDataPointer(), static_cast<int>(vectorB.GetIncrement()), matrix.GetDataPointer(), static_cast<int>(matrix.GetIncrement()));
        }

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::AddSet(ElementType scalar, ConstVectorReference<ElementType, orientation> vector, VectorReference<ElementType, orientation> output)
        {
            output.Fill(scalar);
            AddUpdate(vector, output);
        }

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::AddSet(ConstVectorReference<ElementType, orientation> vectorA, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            output.CopyFrom(vectorA);
            AddUpdate(vectorB, output);
        }

        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::AddUpdate(ElementType scalar, VectorReference<ElementType, orientation> vector)
        {
            UnaryVectorUpdateImplementation(vector, [scalar](ElementType& v) { v += scalar; });
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

        // output = scalarA * vectorA + vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, One, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            output.CopyFrom(vectorB);
            ScaleAddUpdate(scalarA, vectorA, One(), output);
        }

        // vectorC = scalarA * ones + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddSet(ElementType scalarA, OnesVector, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            output.Fill(scalarA);
            ScaleAddUpdate(scalarB, vectorB, One(), output);
        }

        // output = vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddSet(One, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            ScaleSet(scalarB, vectorB, output);
            AddUpdate(vectorA, output);
        }

        // vectorC = scalarA * vectorA + scalarB * vectorB
        template <typename ElementType, VectorOrientation orientation>
        void VectorOperations<ImplementationType::openBlas>::ScaleAddSet(ElementType scalarA, ConstVectorReference<ElementType, orientation> vectorA, ElementType scalarB, ConstVectorReference<ElementType, orientation> vectorB, VectorReference<ElementType, orientation> output)
        {
            ScaleSet(scalarA, vectorA, output);
            ScaleAddUpdate(scalarB, vectorB, One(), output);
        }

#endif // USE_BLAS
    } // namespace Internal
} // namespace math
} // namespace ell

#pragma endregion implementation
