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

// stl
#include<string>

namespace math
{
    /// <summary>
    /// A struct that contains vector matrix operations. Function arguments follow these naming
    /// conventions: r,s,t represent scalars; u,v,w represent vectors; M,A,B represent matrices.
    /// </summary>
    struct NativeOperations
    {
        /// <summary> Gets the implementation name. </summary>
        ///
        /// <returns> The implementation name. </returns>
        static std::string GetImplementationName() { return "Native"; }

        /// <summary> Computes the 0-norm of a vector. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <returns> The 0-norm. </returns>
        template<typename ElementType, VectorOrientation Orientation>
        static ElementType Norm0(ConstVectorReference<ElementType, Orientation>& v);

        /// <summary> Computes the 1-norm of a vector. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <returns> The 1-norm. </returns>
        template<typename ElementType, VectorOrientation Orientation>
        static ElementType Norm1(ConstVectorReference<ElementType, Orientation>& v);

        /// <summary> Computes the 2-norm of a vector (not the squared 2-norm). </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <returns> The 2-norm. </returns>
        template<typename ElementType, VectorOrientation Orientation>
        static ElementType Norm2(ConstVectorReference<ElementType, Orientation>& v);

        /// <summary> Adds a scalar to a vector, v += s. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar being added. </param>
        /// <param name="v"> [in,out] The vector to which the scalar is added. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void Add(ElementType s, VectorReference<ElementType, Orientation>& v);

        /// <summary> Adds a scaled vector to another vector, u += s * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Orientation of the two vectors. </typeparam>
        /// <param name="s"> The scalar that multiplies the right hand side vector v. </param>
        /// <param name="v"> The right hand side vector. </param>
        /// <param name="u"> [in,out] The left hand side vector. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void Add(ElementType s, ConstVectorReference<ElementType, Orientation>& v, VectorReference<ElementType, Orientation>& u);

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
        /// <returns> The dot Multiply. </returns>
        template<typename ElementType, VectorOrientation OrientationV, VectorOrientation OrientationU>
        static ElementType Dot(ConstVectorReference<ElementType, OrientationV>& u, ConstVectorReference<ElementType, OrientationU>& v);

        /// <summary> Calculates the product of a vector and a scalar, v = s * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar that multiplies the vector. </param>
        /// <param name="v"> [in,out] The vector, in any orientation, which is multiplied by s. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void Multiply(ElementType s, VectorReference<ElementType, Orientation>& v);

        /// <summary> Calculates the product of a row vector with a column vector, r = u * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The left vector in row orientation. </param>
        /// <param name="v"> The right vector in column orientation. </param>
        /// <param name="r"> [out] The scalar used to store the result. </param>
        template<typename ElementType>
        static void Multiply(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r);

        /// <summary> Generalized matrix vector multiplication, u = s * M * v + t * u. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="Layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="v"> A column vector, multiplied by t and used to store the result. </param>
        /// <param name="t"> The scalar that multiplies u. </param>
        /// <param name="u"> [in,out] The column vector that multiplies the matrix on the right. </param>
        template<typename ElementType, MatrixLayout Layout>
        static void Multiply(ElementType s, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType t, VectorReference<ElementType, VectorOrientation::column>& u);
    };

#ifdef USE_BLAS
    struct BlasOperations : public NativeOperations 
    {
        /// <summary> Gets the implementation name. </summary>
        ///
        /// <returns> The implementation name. </returns>
        static std::string GetImplementationName() { return "Blas"; }

        /// <summary> Computes the 1-norm of a vector. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <returns> The 1-norm. </returns>
        template<typename ElementType, VectorOrientation Orientation>
        static ElementType Norm1(ConstVectorReference<ElementType, Orientation>& v);

        /// <summary> Computes the 2-norm of a vector (not the squared 2-norm). </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <returns> The 2-norm. </returns>
        template<typename ElementType, VectorOrientation Orientation>
        static ElementType Norm2(ConstVectorReference<ElementType, Orientation>& v);

        /// <summary> Adds a scaled vector to another vector, u += s * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Orientation of the two vectors. </typeparam>
        /// <param name="s"> The scalar that multiplies the right hand side vector v. </param>
        /// <param name="v"> The right hand side vector. </param>
        /// <param name="u"> [in,out] The left hand side vector. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void Add(ElementType s, ConstVectorReference<ElementType, Orientation>& v, VectorReference<ElementType, Orientation>& u);
        using NativeOperations::Add;

        /// <summary>
        /// Calculates a vector dot Multiply (between vectors in any orientation), u * v.
        /// </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="OrientationV"> Orientation of v. </typeparam>
        /// <typeparam name="OrientationU"> Orientation of u. </typeparam>
        /// <param name="u"> The first vector, in any orientation. </param>
        /// <param name="v"> The second vector, in any orientation. </param>
        ///
        /// <returns> The dot Multiply. </returns>
        template<typename ElementType, VectorOrientation OrientationV, VectorOrientation OrientationU>
        static ElementType Dot(ConstVectorReference<ElementType, OrientationV>& u, ConstVectorReference<ElementType, OrientationU>& v);

        /// <summary> Calculates the product of a vector and a scalar, v = v * s. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar that multiplies the vector. </param>
        /// <param name="v"> [in,out] The vector, in any orientation, which is multiplied by s. </param>
        template<typename ElementType, VectorOrientation Orientation>
        static void Multiply(ElementType s, VectorReference<ElementType, Orientation>& v);

        /// <summary> Calculates the product of a row vector with a column vector, r = u * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The left vector in row orientation. </param>
        /// <param name="v"> The right vector in column orientation. </param>
        /// <param name="r"> [out] The scalar used to store the result. </param>
        template<typename ElementType>
        static void Multiply(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r);

        /// <summary> Generalized matrix vector multiplication, u = s * M * v + t * u. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="Layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="v"> A column vector, multiplied by t and used to store the result. </param>
        /// <param name="t"> The scalar that multiplies u. </param>
        /// <param name="u"> [in,out] The column vector that multiplies the matrix on the right. </param>
        template<typename ElementType, MatrixLayout Layout>
        static void Multiply(ElementType s, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType t, VectorReference<ElementType, VectorOrientation::column>& u);
    };
#else
    struct BlasOperations : public NativeOperations {};
#endif
    using Operations = BlasOperations;
}

#include "../tcc/NativeOperations.tcc"

#ifdef USE_BLAS
#include "../tcc/BlasOperations.tcc"
#endif