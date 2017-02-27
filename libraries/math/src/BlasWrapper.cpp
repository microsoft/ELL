////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BlasWrapper.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#if USE_BLAS
#include "BlasWrapper.h"

namespace ell
{
namespace math
{
    namespace Blas
    {
        void Copy(int n, const float* x, int incx, float* y, int incy)
        {
            cblas_scopy(n, x, incx, y, incy);
        }

        void Copy(int n, const double* x, int incx, double* y, int incy)
        {
            cblas_dcopy(n, x, incx, y, incy);
        }

        float Asum(int n, const float* x, int incx)
        {
            return cblas_sasum(n, x, incx);
        }

        double Asum(int n, const double* x, int incx)
        {
            return cblas_dasum(n, x, incx);
        }

        float Nrm2(int n, const float* x, int incx)
        {
            return cblas_snrm2(n, x, incx);
        }

        double Nrm2(int n, const double* x, int incx)
        {
            return cblas_dnrm2(n, x, incx);
        }

        void Scal(int n, float alpha, float* x, int incx)
        {
            cblas_sscal(n, alpha, x, incx);
        }

        void Scal(int n, double alpha, double* x, int incx)
        {
            cblas_dscal(n, alpha, x, incx);
        }

        void Axpy(int n, float alpha, const float* x, int incx, float* y, int incy)
        {
            cblas_saxpy(n, alpha, x, incx, y, incy);
        }

        void Axpy(int n, double alpha, const double* x, int incx, double* y, int incy)
        {
            cblas_daxpy(n, alpha, x, incx, y, incy);
        }

        float Dot(int n, const float* x, int incx, const float* y, int incy)
        {
            return cblas_sdot(n, x, incx, y, incy);
        }

        double Dot(int n, const double* x, int incx, const double* y, int incy)
        {
            return cblas_ddot(n, x, incx, y, incy);
        }

        void Gemv(CBLAS_ORDER order, CBLAS_TRANSPOSE transpose, int m, int n, float alpha, const float* M, int lda, const float* x, int incx, float beta, float* y, int incy)
        {
            cblas_sgemv(order, transpose, m, n, alpha, M, lda, x, incx, beta, y, incy);
        }

        void Gemv(CBLAS_ORDER order, CBLAS_TRANSPOSE transpose, int m, int n, double alpha, const double* M, int lda, const double* x, int incx, double beta, double* y, int incy)
        {
            cblas_dgemv(order, transpose, m, n, alpha, M, lda, x, incx, beta, y, incy);
        }

        void Gemm(CBLAS_ORDER order, CBLAS_TRANSPOSE transposeA, CBLAS_TRANSPOSE transposeB, int m, int n, int k, float alpha, const float* A, int lda, const float* B, int ldb, float beta, float* C, int ldc)
        {
            cblas_sgemm(order, transposeA, transposeB, m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
        }

        void Gemm(CBLAS_ORDER order, CBLAS_TRANSPOSE transposeA, CBLAS_TRANSPOSE transposeB, int m, int n, int k, double alpha, const double* A, int lda, const double* B, int ldb, double beta, double* C, int ldc)
        {
            cblas_dgemm(order, transposeA, transposeB, m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
        }
    }
}
}
#endif
