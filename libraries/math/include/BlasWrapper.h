////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasWrapper.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define USE_BLAS

// OpenBlas
#include "cblas.h"

// stl
#include <cstddef> // size_t

namespace math
{
    namespace Blas
    {
        /// @{
        /// <summary> Wraps the BLAS ASUM function, which computes the 1-norm of a vector . </summary>
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
        /// <summary> Wraps the BLAS NRM2 function, which computes the 2-norm of a vector . </summary>
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
        /// <param name="y"> [in,out] The left-hand-side array, which is modified by this procedure. </param>
        /// <param name="incy"> The Increment of the left-hand-side array. </param>
        void Axpy(int n, float alpha, const float* x, int incx, float* y, int incy);
        void Axpy(int n, double alpha, const double* x, int incx, double* y, int incy);
        /// @}

        /// @{
        /// <summary>
        /// Wraps the BLAS DOT function, which computes the dot Multiply of two vectors.
        /// </summary>
        ///
        /// <param name="n"> The size of each of the arrays that store the vectors. </param>
        /// <param name="x"> Pointer to the first element of the first array. </param>
        /// <param name="incx"> The Increment of the first array. </param>
        /// <param name="y"> [in,out] Pointer to the first element of the second array. </param>
        /// <param name="incy"> The Increment of the second array. </param>
        ///
        /// <returns> The dot Multiply. </returns>
        float Dot(int n, const float* x, int incx, const float* y, int incy);
        double Dot(int n, const double* x, int incx, const double* y, int incy);
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
        /// <param name="x"> The vector x. </param>
        /// <param name="incx"> The increment of vector x. </param>
        /// <param name="beta"> The scalar beta, which multiplies the left-hand side vector y. </param>
        /// <param name="y"> The vector y, multiplied by beta and used to store the result. </param>
        /// <param name="incy"> The incy. </param>
        void Gemv(CBLAS_ORDER order, CBLAS_TRANSPOSE transpose, int m, int n, float alpha, const float* M, int lda, const float* x, int incx, float beta, float* y, int incy);
        void Gemv(CBLAS_ORDER order, CBLAS_TRANSPOSE transpose, int m, int n, double alpha, const double* M, int lda, const double* x, int incx, double beta, double* y, int incy);
        /// @}

    }

}