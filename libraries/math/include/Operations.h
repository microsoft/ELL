////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Operations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix.h"
#include "Vector.h"
#ifdef USE_BLAS
#include "BlasWrapper.h"
#endif

// stl
#include <string>

namespace ell
{
namespace math
{
    /// <summary> Native implementations of static vector/matrix operations that are not implemented in BLAS. </summary>
    struct CommonOperations
    {
        /// <summary> Adds a scalar to a vector, v += s. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar being added. </param>
        /// <param name="v"> [in,out] The vector to which the scalar is added. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Add(ElementType s, VectorReference<ElementType, orientation> v);

        /// <summary> Adds a scalar to a matrix, M += s. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <param name="s"> The scalar being added. </param>
        /// <param name="M"> [in,out] The row major matrix to which the scalar is added. </param>
        template <typename ElementType, MatrixLayout layout>
        static void Add(ElementType s, MatrixReference<ElementType, layout> M);
    };

    /// <summary>
    /// Implementations of static vector/matrix operations that are derived from other (more basic)
    /// operations, which are implemented in a derived class.
    /// </summary>
    ///
    /// <typeparam name="DerivedClass"> The derived class. </typeparam>
    template <class DerivedClass>
    struct DerivedOperations : public CommonOperations
    {
        /// <summary> Generalized matrix matrix addition, C = s * A + t * B. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
        /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
        /// <param name="s"> The scalar that multiplies the first matrix. </param>
        /// <param name="A"> The first matrix. </param>
        /// <param name="t"> The scalar that multiplies the second matrix. </param>
        /// <param name="B"> The second matrix. </param>
        /// <param name="C"> [in,out] A matrix used to store the result in the layout of first matrix. </param>
        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        static void Add(ElementType s, ConstMatrixReference<ElementType, layoutA> A, ElementType t, ConstMatrixReference<ElementType, layoutB> B, MatrixReference<ElementType, layoutA> C);

        /// <summary> Multiplies a row major matrix by a scalar, M *= s. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="M"> [in,out] The row major matrix which is multiplied by s. </param>
        template <typename ElementType, MatrixLayout layout>
        static void Multiply(ElementType s, MatrixReference<ElementType, layout> M);

        /// <summary> Generalized (left-size) matrix row-vector multiplication, u = s * v * M + t * u. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="v"> The row  vector that multiplies the matrix on the left. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="t"> The scalar that multiplies u. </param>
        /// <param name="u"> [in,out] A row vector, multiplied by t and used to store the result. </param>
        template <typename ElementType, MatrixLayout layout>
        static void Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row> v, ConstMatrixReference<ElementType, layout> M, ElementType t, VectorReference<ElementType, VectorOrientation::row> u);

        /// <summary> Multiplies a vector by a scalar s and then adds a scalar b to it, v = s*v + b. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar that multiplies the vector. </param>
        /// <param name="b"> The scalar added to the vector. </param>
        /// <param name="v"> [in,out] The vector being modified. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void MultiplyAdd(ElementType s, ElementType b, VectorReference<ElementType, orientation> v);

        /// <summary> Multiplies a matrix by a scalar s and then adds a scalar b to it, M = s*M + b. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="b"> The scalar added to the matrix. </param>
        /// <param name="M"> [in,out] The matrix being modified. </param>
        template <typename ElementType, MatrixLayout layout>
        static void MultiplyAdd(ElementType s, ElementType b, MatrixReference<ElementType, layout> M);

        /// <summary> Vector vector element wise multiplication, t = u .* v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> Vector orientaton of result vector. </typeparam>
        /// <param name="u"> The first vector. </param>
        /// <param name="v"> The second vector. </param>
        /// <param name="t"> [in,out] The vector used to store the result. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void ElementWiseMultiply(UnorientedConstVectorReference<ElementType> u, UnorientedConstVectorReference<ElementType> v, VectorReference<ElementType, orientation> t);

        /// <summary> Matrix matrix element wise multiplication, C = A .* B. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
        /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
        /// <param name="A"> The first matrix. </param>
        /// <param name="B"> The second matrix. </param>
        /// <param name="C"> [in,out] A matrix used to store the result in the layout of first matrix. </param>
        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        static void ElementWiseMultiply(ConstMatrixReference<ElementType, layoutA> A, ConstMatrixReference<ElementType, layoutB> B, MatrixReference<ElementType, layoutA> C);
    };

    /// <summary> An enum that represent different implementation types. </summary>
    enum class ImplementationType
    {
        native,
        openBlas
    };

    /// <summary> Forward declaration of OperationsImplementation, for subsequent specialization. </summary>
    ///
    /// <typeparam name="implementation"> Type of implementation. </typeparam>
    template <ImplementationType implementation>
    struct OperationsImplementation;

    /// <summary>
    /// Native implementation of vector and matrix operations. Function arguments follow the following
    /// naming conventions: r,s,t represent scalars; u,v,w represent vectors; M,A,B represent matrices.
    /// </summary>
    template <>
    struct OperationsImplementation<ImplementationType::native> : public DerivedOperations<OperationsImplementation<ImplementationType::native>>
    {
        using CommonOperations::Add;
        using DerivedOperations<OperationsImplementation<ImplementationType::native>>::Add;
        using DerivedOperations<OperationsImplementation<ImplementationType::native>>::Multiply;
        using DerivedOperations<OperationsImplementation<ImplementationType::native>>::MultiplyAdd;

        /// <summary> Gets the implementation name. </summary>
        ///
        /// <returns> The implementation name. </returns>
        static std::string GetImplementationName() { return "Native"; }

        ///// <summary> Columnwise sum of a matrix. </summary>
        /////
        ///// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        ///// <typeparam name="layout"> Matrix layout. </typeparam>
        ///// <param name="M"> The matrix. </param>
        ///// <param name="u"> [in,out] A column vector, used to store the result. </param>
        template <typename ElementType, MatrixLayout layout>
        static void ColumnWiseSum(ConstMatrixReference<ElementType, layout> M, VectorReference<ElementType, VectorOrientation::row> u);

        /// <summary> Adds a scaled vector to another vector, u += s * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> orientation of the two vectors. </typeparam>
        /// <param name="s"> The scalar that multiplies the right hand side vector v. </param>
        /// <param name="v"> The right hand side vector. </param>
        /// <param name="u"> [in,out] The left hand side vector. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Add(ElementType s, ConstVectorReference<ElementType, orientation> v, VectorReference<ElementType, orientation> u);

        /// <summary>
        /// Calculates a vector dot product (between vectors in any orientation), u * v.
        /// </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The first vector, in any orientation. </param>
        /// <param name="v"> The second vector, in any orientation. </param>
        ///
        /// <returns> The dot Multiply. </returns>
        template <typename ElementType>
        static ElementType Dot(UnorientedConstVectorReference<ElementType> u, UnorientedConstVectorReference<ElementType> v);

        /// <summary> Multiplies a vector by a scalar, v *= s. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar that multiplies the vector. </param>
        /// <param name="v"> [in,out] The vector, in any orientation, which is multiplied by s. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Multiply(ElementType s, VectorReference<ElementType, orientation> v);

        /// <summary> Calculates the product of a row vector with a column vector, r = u * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The left vector in row orientation. </param>
        /// <param name="v"> The right vector in column orientation. </param>
        /// <param name="r"> [out] The scalar used to store the result. </param>
        template <typename ElementType>
        static void Multiply(ConstVectorReference<ElementType, VectorOrientation::row> u, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType& r);

        /// <summary> Generalized matrix column-vector multiplication, u = s * M * v + t * u. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="v"> The column vector that multiplies the matrix on the right. </param>
        /// <param name="t"> The scalar that multiplies the left hand side vector u. </param>
        /// <param name="u"> [in,out] A column vector, multiplied by t and used to store the result. </param>
        template <typename ElementType, MatrixLayout layout>
        static void Multiply(ElementType s, ConstMatrixReference<ElementType, layout> M, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType t, VectorReference<ElementType, VectorOrientation::column> u);

        /// <summary> Generalized matrix matrix multiplication, C = s * A * B + t * C. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
        /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="A"> The first matrix. </param>
        /// <param name="B"> The second matrix. </param>
        /// <param name="t"> The scalar that multiplies C. </param>
        /// <param name="C"> [in,out] A matrix, multiplied by t and used to store the result in the layout of first matrix. </param>
        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        static void Multiply(ElementType s, ConstMatrixReference<ElementType, layoutA> A, ConstMatrixReference<ElementType, layoutB> B, ElementType t, MatrixReference<ElementType, layoutA> C);
    };

#ifdef USE_BLAS
    /// OpenBLAS implementation of vector and matrix operations. Function arguments follow the following
    /// naming conventions: r,s,t represent scalars; u,v,w represent vectors; M,A,B represent matrices.
    template <>
    struct OperationsImplementation<ImplementationType::openBlas> : public DerivedOperations<OperationsImplementation<ImplementationType::openBlas>>
    {
        using CommonOperations::Add;
        using DerivedOperations<OperationsImplementation<ImplementationType::openBlas>>::Add;
        using DerivedOperations<OperationsImplementation<ImplementationType::openBlas>>::Multiply;
        using DerivedOperations<OperationsImplementation<ImplementationType::openBlas>>::MultiplyAdd;

        /// <summary> Gets the implementation name. </summary>
        ///
        /// <returns> The implementation name. </returns>
        static std::string GetImplementationName() { return "Blas"; }

        ///// <summary> Columnwise sum of a matrix. </summary>
        /////
        ///// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        ///// <typeparam name="layout"> Matrix layout. </typeparam>
        ///// <param name="s"> The scalar that multiplies the matrix. </param>
        ///// <param name="u"> [in,out] A column vector, multiplied by t and used to store the result. </param>
        template <typename ElementType, MatrixLayout layout>
        static void ColumnWiseSum(ConstMatrixReference<ElementType, layout> M, VectorReference<ElementType, VectorOrientation::row> u);

        /// <summary> Adds a scaled vector to another vector, u += s * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> orientation of the two vectors. </typeparam>
        /// <param name="s"> The scalar that multiplies the right hand side vector v. </param>
        /// <param name="v"> The right hand side vector. </param>
        /// <param name="u"> [in,out] The left hand side vector. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Add(ElementType s, ConstVectorReference<ElementType, orientation> v, VectorReference<ElementType, orientation> u);

        /// <summary>
        /// Calculates a vector dot Multiply (between vectors in any orientation), u * v.
        /// </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The first vector, in any orientation. </param>
        /// <param name="v"> The second vector, in any orientation. </param>
        ///
        /// <returns> The dot Multiply. </returns>
        template <typename ElementType>
        static ElementType Dot(UnorientedConstVectorReference<ElementType> u, UnorientedConstVectorReference<ElementType> v);

        /// <summary> Calculates the product of a vector and a scalar, v = v * s. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="orientation"> Vector orientation. </typeparam>
        /// <param name="s"> The scalar that multiplies the vector. </param>
        /// <param name="v"> [in,out] The vector, in any orientation, which is multiplied by s. </param>
        template <typename ElementType, VectorOrientation orientation>
        static void Multiply(ElementType s, VectorReference<ElementType, orientation> v);

        /// <summary> Calculates the product of a row vector with a column vector, r = u * v. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <param name="u"> The left vector in row orientation. </param>
        /// <param name="v"> The right vector in column orientation. </param>
        /// <param name="r"> [out] The scalar used to store the result. </param>
        template <typename ElementType>
        static void Multiply(ConstVectorReference<ElementType, VectorOrientation::row> u, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType& r);

        /// <summary> Generalized matrix column-vector multiplication, u = s * M * v + t * u. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="v"> The column vector that multiplies the matrix on the right. </param>
        /// <param name="t"> The scalar that multiplies the left hand side vector u. </param>
        /// <param name="u"> [in,out] A column vector, multiplied by t and used to store the result. </param>
        template <typename ElementType, MatrixLayout layout>
        static void Multiply(ElementType s, ConstMatrixReference<ElementType, layout> M, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType t, VectorReference<ElementType, VectorOrientation::column> u);

        /// <summary> Generalized matrix row-vector multiplication, u = s * v * M + t * u. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix and vector element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="v"> The row  vector that multiplies the matrix on the left. </param>
        /// <param name="M"> The matrix. </param>
        /// <param name="t"> The scalar that multiplies u. </param>
        /// <param name="u"> [in,out] A row vector, multiplied by t and used to store the result. </param>
        template <typename ElementType, MatrixLayout layout>
        static void Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row> v, const ConstMatrixReference<ElementType, layout> M, ElementType t, VectorReference<ElementType, VectorOrientation::row> u);

        /// <summary> Generalized matrix matrix multiplication, C = s * A * B + t * C. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layoutA"> Matrix layout of first matrix. </typeparam>
        /// <typeparam name="layoutB"> Matrix layout of second matrix. </typeparam>
        /// <param name="s"> The scalar that multiplies the matrix. </param>
        /// <param name="A"> The first matrix. </param>
        /// <param name="B"> The second matrix. </param>
        /// <param name="t"> The scalar that multiplies C. </param>
        /// <param name="u"> [in,out] A matrix, multiplied by t and used to store the result, in layout of first matrix. </param>
        template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
        static void Multiply(ElementType s, ConstMatrixReference<ElementType, layoutA> A, ConstMatrixReference<ElementType, layoutB> B, ElementType t, MatrixReference<ElementType, layoutA> C);
    };

    using Operations = OperationsImplementation<ImplementationType::openBlas>;
#else
    /// Native implementation of vector and matrix operations. Function arguments follow the following
    /// naming conventions: r,s,t represent scalars; u,v,w represent vectors; M,A,B represent matrices.
    template <>
    struct OperationsImplementation<ImplementationType::openBlas> : public OperationsImplementation<ImplementationType::native>
    {
    };

    using Operations = OperationsImplementation<ImplementationType::native>;
#endif // USE_BLAS
}
}

#include "../tcc/Operations.tcc"
