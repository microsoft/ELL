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
        float Asum(size_t size, const float* ptr, size_t Increment)
        {
            auto result = cblas_sasum(static_cast<int>(size), ptr, static_cast<int>(Increment));
            return result;
        }

        double Asum(size_t size, double* ptr, size_t Increment)
        {
            auto result = cblas_dasum(static_cast<int>(size), ptr, static_cast<int>(Increment));
            return result;
        }

        float Nrm2(size_t size, const float* ptr, size_t Increment)
        {
            auto result = cblas_snrm2(static_cast<int>(size), ptr, static_cast<int>(Increment));
            return result;
        }

        double Nrm2(size_t size, double* ptr, size_t Increment)
        {
            auto result = cblas_dnrm2(static_cast<int>(size), ptr, static_cast<int>(Increment));
            return result;
        }

        void Scal(size_t size, float alpha, float* pLhs, size_t lhsIncrement)
        {
            cblas_sscal(static_cast<int>(size), alpha, pLhs, static_cast<int>(lhsIncrement));
        }

        void Scal(size_t size, double alpha, double* pLhs, size_t lhsIncrement)
        {
            cblas_dscal(static_cast<int>(size), alpha, pLhs, static_cast<int>(lhsIncrement));
        }

        void Axpy(size_t size, float alpha, const float * pRhs, size_t rhsIncrement, float * pLhs, size_t lhsIncrement)
        {
            cblas_saxpy(static_cast<int>(size), alpha, pRhs, static_cast<int>(rhsIncrement), pLhs, static_cast<int>(lhsIncrement));
        }

        void Axpy(size_t size, double alpha, const double * pRhs, size_t rhsIncrement, double * pLhs, size_t lhsIncrement)
        {
            cblas_daxpy(static_cast<int>(size), alpha, pRhs, static_cast<int>(rhsIncrement), pLhs, static_cast<int>(lhsIncrement));
        }

        float Dot(size_t size, const float * pFirst, size_t firstIncrement, const float * pSecond, size_t secondIncrement)
        {
            auto result = cblas_sdot(static_cast<int>(size), pFirst, static_cast<int>(firstIncrement), pSecond, static_cast<int>(secondIncrement));
            return result;
        }

        double Dot(size_t size, const double* pFirst, size_t firstIncrement, const double* pSecond, size_t secondIncrement)
        {
            auto result = cblas_ddot(static_cast<int>(size), pFirst, static_cast<int>(firstIncrement), pSecond, static_cast<int>(secondIncrement));
            return result;
        }
    }
}

