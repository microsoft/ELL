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
    /// <summary>
    /// A struct that contains vector matrix operations. Function arguments follow these naming
    /// conventions: r,s,t represent scalars; u,v represent vectors; A,B represent matrices.
    /// </summary>
    struct Operations
    {
        /// <summary> Adds a scalar to a vector, v += s. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar being added. </param>
        /// <param name="r"> [in,out] The vector to which the scalar is added. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void AddTo(ElementType s, VectorReference<ElementType, Orientation>& r);

        /// <summary> Adds a scaled vector to another vector, v += s * u. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Orientation of the two vectors. </typeparam>
        /// <param name="s"> The scalar that multiplies the right hand side vector. </param>
        /// <param name="u"> The vector being added to lefr hand side. </param>
        /// <param name="v"> [in,out] The vector to which the right hand side is added. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void AddTo(ElementType s, ConstVectorReference<ElementType, Orientation>& u, VectorReference<ElementType, Orientation>& v);

        /// <summary>
        /// Calculates a vector dot product (between vectors in any orientation), u * v.
        /// </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="OrientationV"> Orientation of v. </typeparam>
        /// <typeparam name="OrientationU"> Orientation of u. </typeparam>
        /// <param name="u"> The first vector, in any orientation. </param>
        /// <param name="v"> The second vector, in any orientation. </param>
        ///
        /// <returns> The dot product. </returns>
        template<typename ElementType, VectorOrientation OrientationV, VectorOrientation OrientationU>
        static ElementType Dot(ConstVectorReference<ElementType, OrientationV>& u, ConstVectorReference<ElementType, OrientationU>& v);

        /// <summary> Calculates the product of a row vector with a column vector, r = u * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The left vector in row orientation. </param>
        /// <param name="v"> The right vector in column orientation. </param>
        /// <param name="r"> [out] The scalar used to store the result. </param>
        template<typename ElementType>
        static void Product(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r);

        /// <summary> Generalized matrix vector product, v = s * A * u + t * v. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="Layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="A"> The matrix. </param>
        /// <param name="u"> The column vector that multiplies the matrix on the right. </param>
        /// <param name="t"> The scalar that multiplies v. </param>
        /// <param name="v"> [in,out] A column vector, multiplied by t and used to store the result. </param>
        template<typename ElementType, MatrixLayout Layout>
        static void Product(ElementType s, ConstMatrixReference<ElementType, Layout>& A, ConstVectorReference<ElementType, VectorOrientation::column>& u, ElementType t, VectorReference<ElementType, VectorOrientation::column>& v);
    };
}

#include "../tcc/Operations.tcc"
