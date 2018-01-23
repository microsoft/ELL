////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BlasWrapper.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "BlasWrapper.h"
#if USE_BLAS
#include "cblas.h"
#endif
// stl
#include <thread> // for hardware_concurrency()

namespace ell
{
namespace math
{
    namespace Blas
    {
        int GetCBlasMatrixOrder(MatrixLayout order)
        {
            switch (order) {
            case MatrixLayout::rowMajor:
#if USE_BLAS
                return CBLAS_ORDER::CblasRowMajor;
#else
                // We still want to be able to generate code for another platform (like Raspberry Pi) that can use OpenBLAS
                // even the the machine we are building on doesn't have cblas.h.
                return 101;
#endif
            case MatrixLayout::columnMajor:
#if USE_BLAS
                return CBLAS_ORDER::CblasColMajor;
#else
                return 102;
#endif
            }
            return static_cast<int>(order);
        }

        int GetCBlasMatrixTranspose(MatrixTranspose transpose)
        {
            switch (transpose) {
            case MatrixTranspose::transpose:
#if USE_BLAS
                return CBLAS_TRANSPOSE::CblasTrans;
#else
                return 112;
#endif
            case MatrixTranspose::noTranspose:
#if USE_BLAS
                return CBLAS_TRANSPOSE::CblasNoTrans;
#else
                return 111;
#endif
            }
            return static_cast<int>(transpose);
        }

        void SetNumThreads(int numThreads)
        {
            if (numThreads == 0)
            {
                numThreads = std::thread::hardware_concurrency();
            }
#ifdef OPENBLAS_CONST
            openblas_set_num_threads(numThreads);
#endif
        }

#if USE_BLAS
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

        void Ger(MatrixLayout order, int m, int n, float alpha, const float* x, int incx, const float* y, int incy, float* M, int lda)
        {
            cblas_sger(static_cast<CBLAS_ORDER>(GetCBlasMatrixOrder(order)), m, n, alpha, x, incx, y, incy, M, lda);
        }

        void Ger(MatrixLayout order, int m, int n, double alpha, const double* x, int incx, const double* y, int incy, double* M, int lda)
        {
            cblas_dger(static_cast<CBLAS_ORDER>(GetCBlasMatrixOrder(order)), m, n, alpha, x, incx, y, incy, M, lda);
        }

        void Gemv(MatrixLayout order, MatrixTranspose transpose, int m, int n, float alpha, const float* M, int lda, const float* x, int incx, float beta, float* y, int incy)
        {
            cblas_sgemv(static_cast<CBLAS_ORDER>(GetCBlasMatrixOrder(order)), static_cast<CBLAS_TRANSPOSE>(GetCBlasMatrixTranspose(transpose)), m, n, alpha, M, lda, x, incx, beta, y, incy);
        }

        void Gemv(MatrixLayout order, MatrixTranspose transpose, int m, int n, double alpha, const double* M, int lda, const double* x, int incx, double beta, double* y, int incy)
        {
            cblas_dgemv(static_cast<CBLAS_ORDER>(GetCBlasMatrixOrder(order)), static_cast<CBLAS_TRANSPOSE>(GetCBlasMatrixTranspose(transpose)), m, n, alpha, M, lda, x, incx, beta, y, incy);
        }

        void Gemm(MatrixLayout order, MatrixTranspose transposeA, MatrixTranspose transposeB, int m, int n, int k, float alpha, const float* A, int lda, const float* B, int ldb, float beta, float* C, int ldc)
        {
            cblas_sgemm(static_cast<CBLAS_ORDER>(GetCBlasMatrixOrder(order)), static_cast<CBLAS_TRANSPOSE>(GetCBlasMatrixTranspose(transposeA)), static_cast<CBLAS_TRANSPOSE>(GetCBlasMatrixTranspose(transposeB)), m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
        }

        void Gemm(MatrixLayout order, MatrixTranspose transposeA, MatrixTranspose transposeB, int m, int n, int k, double alpha, const double* A, int lda, const double* B, int ldb, double beta, double* C, int ldc)
        {
            cblas_dgemm(static_cast<CBLAS_ORDER>(GetCBlasMatrixOrder(order)), static_cast<CBLAS_TRANSPOSE>(GetCBlasMatrixTranspose(transposeA)), static_cast<CBLAS_TRANSPOSE>(GetCBlasMatrixTranspose(transposeB)), m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
        }
#endif
    }
}
}
