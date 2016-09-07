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

        void Axpy(size_t size, float alpha, const float * pRhs, size_t rhsStride, float * pLhs, size_t lhsStride)
        {
            // TODO
        }

        void Axpy(size_t size, double alpha, const double * pRhs, size_t rhsStride, double * pLhs, size_t lhsStride)
        {
            // TODO
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

