////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasWrapper.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BlasWrapper.h"

namespace math
{
    namespace Blas
    {
        float Nrm2(size_t size, const float * ptr, size_t stride)
        {
            return 0.0f; // TODO
        }

        double Nrm2(size_t size, double * ptr, size_t stride)
        {
            return 0.0; // TODO
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
            return 0.0f; // TODO
        }

        double Dot(size_t size, const double * pFirst, size_t firstStride, const double * pSecond, size_t secondStride)
        {
            return 0.0; // TODO
        }
    }
}

