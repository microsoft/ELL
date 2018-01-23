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

// utilities
#include "TypeTraits.h"

// stl
#include <string>
#include <ostream>

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
    auto operator*(double scalar, ConstVectorReference<ElementType, orientation> vector)->TransformedConstVectorReference<ElementType, orientation, ScaleFunction<ElementType>>;

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
    struct OnesVector {};

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

        template<>
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
        template<>
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
        template<>
        struct VectorOperations<ImplementationType::openBlas> : public VectorOperations<ImplementationType::native>
        {};

#endif // USE_BLAS
    }
}
}

#include "../tcc/VectorOperations.tcc"
