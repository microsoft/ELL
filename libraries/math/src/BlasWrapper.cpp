////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasWrapper.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BlasWrapper.h"

// OpenBlas
#include "cblas.h"

namespace math
{
    namespace Blas
    {
        float Asum(size_t size, const float* ptr, size_t stride)
        {
            auto result = cblas_sasum(static_cast<int>(size), ptr, static_cast<int>(stride));
            return result;
        }

        double Asum(size_t size, double* ptr, size_t stride)
        {
            auto result = cblas_dasum(static_cast<int>(size), ptr, static_cast<int>(stride));
            return result;
        }

        float Nrm2(size_t size, const float* ptr, size_t stride)
        {
            auto result = cblas_snrm2(static_cast<int>(size), ptr, static_cast<int>(stride));
            return result;
        }

        double Nrm2(size_t size, double* ptr, size_t stride)
        {
            auto result = cblas_dnrm2(static_cast<int>(size), ptr, static_cast<int>(stride));
            return result;
        }

        void Scal(size_t size, float alpha, float* pLhs, size_t lhsStride)
        {
            cblas_sscal(static_cast<int>(size), alpha, pLhs, static_cast<int>(lhsStride));
        }

        void Scal(size_t size, double alpha, double* pLhs, size_t lhsStride)
        {
            cblas_dscal(static_cast<int>(size), alpha, pLhs, static_cast<int>(lhsStride));
        }

        void Axpy(size_t size, float alpha, const float * pRhs, size_t rhsStride, float * pLhs, size_t lhsStride)
        {
            cblas_saxpy(static_cast<int>(size), alpha, pRhs, static_cast<int>(rhsStride), pLhs, static_cast<int>(lhsStride));
        }

        void Axpy(size_t size, double alpha, const double * pRhs, size_t rhsStride, double * pLhs, size_t lhsStride)
        {
            cblas_daxpy(static_cast<int>(size), alpha, pRhs, static_cast<int>(rhsStride), pLhs, static_cast<int>(lhsStride));
        }

        float Dot(size_t size, const float * pFirst, size_t firstStride, const float * pSecond, size_t secondStride)
        {
            auto result = cblas_sdot(static_cast<int>(size), pFirst, static_cast<int>(firstStride), pSecond, static_cast<int>(secondStride));
            return result;
        }

        double Dot(size_t size, const double* pFirst, size_t firstStride, const double* pSecond, size_t secondStride)
        {
            auto result = cblas_ddot(static_cast<int>(size), pFirst, static_cast<int>(firstStride), pSecond, static_cast<int>(secondStride));
            return result;
        }
    }
}

