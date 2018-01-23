////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BlasWrapper.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix.h"

// stl
#include <cstddef> // size_t

namespace ell
{
namespace math
{
    namespace Blas
    {
        /// <summary> Map the given enum value to OpenBLAS's definition of CBLAS_ORDER.</summary>
        ///
        /// <param name="order">The ELL definition of MatrixLayout to be mapped. </param>
        int GetCBlasMatrixOrder(MatrixLayout order);

        /// <summary> Map the given enum value to OpenBLAS's definition of CBLAS_TRANSPOSE.</summary>
        ///
        /// <param name="order">The ELL definition of MatrixTranspose to be mapped. </param>
        int GetCBlasMatrixTranspose(MatrixTranspose transpose);

        /// <summary> Sets the number of threads. </summary>
        ///
        /// <param name="numThreads"> The number of threads. </param>
        void SetNumThreads(int numThreads);

        /// @{
        /// <summary> Wraps the BLAS COPY function, which copies a vector. </summary>
        /// <param name="n"> The size of each of the arrays that store the vectors. </param>
        /// <param name="x"> Pointer to the first element of the right-hand-side array. </param>
        /// <param name="incx"> The Increment of the right-hand-side array. </param>
        /// <param name="y"> Pointer to the left-hand-side array, which is modified by this procedure. </param>
        /// <param name="incy"> The Increment of the left-hand-side array. </param>
        void Copy(int n, const float* x, int incx, float* y, int incy);
        void Copy(int n, const double* x, int incx, double* y, int incy);
        /// @}

        /// @{
        /// <summary> Wraps the BLAS ASUM function, which computes the 1-norm of a vector. </summary>
        ///
        /// <param name="n"> The size of the array that stores the vector. </param>
        /// <param name="x"> Pointer to the first element of the array. </param>
        /// <param name="incx"> The Increment of the array. </param>
        ///
        /// <returns> The 1-norm of the vector. </returns>
        float Asum(int n, const float* x, int incx);
        double Asum(int n, const double* x, int incx);
        /// @}

        /// @{
        /// <summary> Wraps the BLAS NRM2 function, which computes the 2-norm of a vector. </summary>
        ///
        /// <param name="n"> The size of the array that stores the vector. </param>
        /// <param name="x"> Pointer to the first element of the array. </param>
        /// <param name="incx"> The Increment of the array. </param>
        ///
        /// <returns> The 2-norm of the vector. </returns>
        float Nrm2(int n, const float* x, int incx);
        double Nrm2(int n, const double* x, int incx);
        /// @}

        /// @{
        /// <summary> Wraps the BLAS SCAL function, which multiplies a vector by a scalar. </summary>
        ///
        /// <param name="n"> The size of the array that stores the vector. </param>
        /// <param name="alpha"> The scalar that multiplies the vector. </param>
        /// <param name="x"> [in,out] Pointer to the first element of the array. </param>
        /// <param name="incx"> The Increment of the array. </param>
        void Scal(int n, float alpha, float* x, int incx);
        void Scal(int n, double alpha, double* x, int incx);
        /// @}

        /// @{
        /// <summary> Wraps the BLAS AXPY function: v += alpha * u. </summary>
        ///
        /// <param name="n"> The size of each of the arrays that store the vectors. </param>
        /// <param name="alpha"> The scalar that multiplies the right-hand-side array. </param>
        /// <param name="x"> Pointer to the first element of the right-hand-side array. </param>
        /// <param name="incx"> The Increment of the right-hand-side array. </param>
        /// <param name="y"> Pointer to the left-hand-side array, which is modified by this procedure. </param>
        /// <param name="incy"> The Increment of the left-hand-side array. </param>
        void Axpy(int n, float alpha, const float* x, int incx, float* y, int incy);
        void Axpy(int n, double alpha, const double* x, int incx, double* y, int incy);
        /// @}

        /// @{
        /// <summary>
        /// Wraps the BLAS DOT function, which computes the dot product of two vectors.
        /// </summary>
        ///
        /// <param name="n"> The size of each of the arrays that store the vectors. </param>
        /// <param name="x"> Pointer to the first element of the first array. </param>
        /// <param name="incx"> The Increment of the first array. </param>
        /// <param name="y"> Pointer to the first element of the second array. </param>
        /// <param name="incy"> The Increment of the second array. </param>
        ///
        /// <returns> The vector dot product. </returns>
        float Dot(int n, const float* x, int incx, const float* y, int incy);
        double Dot(int n, const double* x, int incx, const double* y, int incy);
        /// @}

         /// @{
        /// <summary>
        /// Wraps the BLAS GER function, which computes the outer product of two vectors.
        /// </summary>
        ///
        /// <param name="order"> Row major or column major. </param>
        /// <param name="m"> Number of matrix rows. </param>
        /// <param name="n"> Number of matrix columns. </param>
        /// <param name="alpha"> The scalar alpha, which multiplies the matrix-vector Multiply. </param>
        /// <param name="x"> Pointer to the first element of the first array. </param>
        /// <param name="incx"> The Increment of the first array. </param>
        /// <param name="y"> Pointer to the first element of the second array. </param>
        /// <param name="incy"> The Increment of the second array. </param>
        /// <param name="M"> The left-hand side matrix M. </param>
        /// <param name="lda"> The matrix increment. </param>
        ///
        /// <returns> The dot Multiply. </returns>
        void Ger(MatrixLayout order, int m, int n, float alpha, const float* x, int incx, const float* y, int incy, float* M, int lda);
        void Ger(MatrixLayout order, int m, int n, double alpha, const double* x, int incx, const double* y, int incy, double* M, int lda);
        /// @}

        /// @{
        /// <summary> Wraps the BLAS GEMV function, which implements generalized matrix vector multiplication, y = alpha*M*x + beta*y. </summary>
        ///
        /// <param name="order"> Row major or column major. </param>
        /// <param name="transpose"> Whether or not to transpose the matrix M. </param>
        /// <param name="m"> Number of matrix rows. </param>
        /// <param name="n"> Number of matrix columns. </param>
        /// <param name="alpha"> The scalar alpha, which multiplies the matrix-vector Multiply. </param>
        /// <param name="M"> The matrix M. </param>
        /// <param name="lda"> The matrix increment. </param>
        /// <param name="x"> Pointer to the first element of the vector x. </param>
        /// <param name="incx"> The increment of vector x. </param>
        /// <param name="beta"> The scalar beta, which multiplies the left-hand side vector y. </param>
        /// <param name="y"> Pointer to the first element of the vector y, multiplied by beta and used to store the result. </param>
        /// <param name="incy"> The incy. </param>
        void Gemv(MatrixLayout order, MatrixTranspose transpose, int m, int n, float alpha, const float* M, int lda, const float* x, int incx, float beta, float* y, int incy);
        void Gemv(MatrixLayout order, MatrixTranspose transpose, int m, int n, double alpha, const double* M, int lda, const double* x, int incx, double beta, double* y, int incy);
        /// @}

        /// @{
        /// <summary> Wraps the BLAS GEMM function, which implements generalized matrix matric multiplication, C = alpha*A*B + beta*C. </summary>
        ///
        /// <param name="order"> Row major or column major. </param>
        /// <param name="transposeA"> Whether or not to transpose the matrix A. </param>
        /// <param name="transposeB"> Whether or not to transpose the matrix B. </param>
        /// <param name="m"> Number of matrix rows in A and C. </param>
        /// <param name="n"> Number of matrix columns in B and C. </param>
        /// <param name="k"> Number of matrix columns in A and matrix rows in B. </param>
        /// <param name="alpha"> The scalar alpha, which multiplies A * B. </param>
        /// <param name="A"> The matrix A. </param>
        /// <param name="lda"> The matrix increment for A. </param>
        /// <param name="B"> The matrix B. </param>
        /// <param name="lda"> The matrix increment for B. </param>
        /// <param name="beta"> The scalar beta, which multiplies the matrix C. </param>
        /// <param name="C"> The matrix C. </param>
        /// <param name="ldc"> The matrix increment for C. </param>
        void Gemm(MatrixLayout order, MatrixTranspose transposeA, MatrixTranspose transposeB, int m, int n, int k, float alpha, const float* A, int lda, const float* B, int ldb, float beta, float* C, int ldc);
        void Gemm(MatrixLayout order, MatrixTranspose transposeA, MatrixTranspose transposeB, int m, int n, int k, double alpha, const double* A, int lda, const double* B, int ldb, double beta, double* C, int ldc);
        /// @}
    }
}
}
