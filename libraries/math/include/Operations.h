////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Operations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"
#include "Matrix.h"

namespace math
{
    /// <summary> A struct that holds all of the binary Vector operations. </summary>
    struct Operations
    {
        /// <summary> Adds a scalar to a vector. </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="rhsScalar"> The scalar being added. </param>
        /// <param name="lhsVector"> [in,out] The vector to which the scalar is added. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void AddTo(ElementType rhsScalar, VectorReference<ElementType, Orientation>& lhsVector);

        /// <summary> Adds a scaled vector to another vector. </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <typeparam name="Orientation"> Orientation of the two vectors. </typeparam>
        /// <param name="rhsScalar"> The scalar that multiplies the rhs vector. </param>
        /// <param name="rhsVector"> The vector being added to lhs. </param>
        /// <param name="lhsVector"> [in,out] The vector to which the rhs is added. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void AddTo(ElementType rhsScalar, ConstVectorReference<ElementType, Orientation>& rhsVector, VectorReference<ElementType, Orientation>& lhsVector);

        /// <summary> Calculates a vector dot product (between vectors in any orientation). </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="vector1"> The first vector, in any orientation. </param>
        /// <param name="vector2"> The second vector, in any orientation. </param>
        ///
        /// <returns> The dot product. </returns>
        template<typename ElementType, VectorOrientation Orientation1, VectorOrientation Orientation2>
        static ElementType Dot(ConstVectorReference<ElementType, Orientation1>& vector1, ConstVectorReference<ElementType, Orientation2>& vector2);

        /// <summary> Calculates the product of a row vector with a column vector. </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="rhsVector1"> The left vector on the rhs, in row orientation. </param>
        /// <param name="rhsVector2"> The right vector on the rhs, in column orientation. </param>
        /// <param name="lhsScalar"> [out] The lhs scalar used to store the result. </param>
        template<typename ElementType>
        static void Product(ConstVectorReference<ElementType, VectorOrientation::row>& rhsVector1, ConstVectorReference<ElementType, VectorOrientation::column>& rhsVector2, ElementType& lhsScalar);

        /// <summary> Matrix vector product, y = alpha * M * x + beta y. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="Layout"> Matrix layout. </typeparam>
        /// <param name="alpha"> The scalar alpha that multiplies the right hand side. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="x"> The column vector x that multiplies the matrix on the right. </param>
        /// <param name="beta"> The scalar beta that multiplies the left hand side vector y. </param>
        /// <param name="y"> The column vector y, multiplied by beta, also used to store the result. </param>
        template<typename ElementType, MatrixLayout Layout>
        static void Product(ElementType alpha, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& x, ElementType beta, VectorReference<ElementType, VectorOrientation::column>& y);

        /// <summary> Matrix vector product, y = alpha * M * x + beta y. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="Layout"> Matrix layout. </typeparam>
        /// <param name="alpha"> The scalar alpha that multiplies the right hand side. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="x"> The row vector x that multiplies the matrix on the left. </param>
        /// <param name="beta"> The scalar beta that multiplies the left hand side vector y. </param>
        /// <param name="y"> The row vector y, multiplied by beta, also used to store the result. </param>
        template<typename ElementType, MatrixLayout Layout>
        static void Product(ElementType alpha, ConstVectorReference<ElementType, VectorOrientation::row>& x, ConstMatrixReference<ElementType, Layout>& M, ElementType beta, VectorReference<ElementType, VectorOrientation::row>& y);

    };
}

#include "../tcc/Operations.tcc"
