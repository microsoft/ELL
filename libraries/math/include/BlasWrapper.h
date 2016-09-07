////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasWrapper.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstddef> // size_t

namespace math
{
    namespace Blas
    {
        /// <summary> Wraps the BLAS NRM2 function, which computes the 2-norm of a vector . </summary>
        ///
        /// <param name="size"> The size of the vector. </param>
        /// <param name="ptr"> Pointer to the first element of the array. </param>
        /// <param name="stride"> The stride of the array. </param>
        ///
        /// <returns> The 2-norm of the vector. </returns>
        float Nrm2(size_t size, const float* ptr, size_t stride);

        /// <summary> Wraps the BLAS NRM2 function, which computes the 2-norm of a vector . </summary>
        ///
        /// <param name="size"> The size of the vector. </param>
        /// <param name="ptr"> Pointer to the first element of the array. </param>
        /// <param name="stride"> The stride of the array. </param>
        ///
        /// <returns> The 2-norm of the vector. </returns>
        double Nrm2(size_t size, double* ptr, size_t stride);

        /// <summary> Wraps the BLAS AXPY function: v += alpha * u </summary>
        ///
        /// <param name="size"> The size of the vectors. </param>
        /// <param name="alpha"> The scalar that multiplies the right-hand-side array. </param>
        /// <param name="pRhs"> Pointer to the first element of the right-hand-side array. </param>
        /// <param name="rhsStride"> The stride of the right-hand-side array. </param>
        /// <param name="pLhs"> [in,out] Pointer to the first element of the left-hand-side array, which is modified by this procedure. </param>
        /// <param name="lhsStride"> The stride of the left-hand-side array. </param>
        void Axpy(size_t size, float alpha, const float* pRhs, size_t rhsStride, float* pLhs, size_t lhsStride);

        /// <summary> Wraps the BLAS AXPY function: v += alpha * u </summary>
        ///
        /// <param name="size"> The size of the vectors. </param>
        /// <param name="alpha"> The scalar that multiplies the right-hand-side array. </param>
        /// <param name="pRhs"> Pointer to the first element of the right-hand-side array. </param>
        /// <param name="rhsStride"> The stride of the right-hand-side array. </param>
        /// <param name="pLhs"> [in,out] The left-hand-side array, which is modified by this procedure. </param>
        /// <param name="lhsStride"> The stride of the left-hand-side array. </param>
        void Axpy(size_t size, double alpha, const double* pRhs, size_t rhsStride, double* pLhs, size_t lhsStride);

        /// <summary> Wraps the BLAS DOT function, which computes the dot product of two vectors. </summary>
        ///
        /// <param name="size"> The size of the vectors. </param>
        /// <param name="pFirst"> Pointer to the first element of the first array. </param>
        /// <param name="firstStride"> The stride of the first array. </param>
        /// <param name="pSecond"> Pointer to the first element of the second array. </param>
        /// <param name="secondStride"> The stride of the second array. </param>
        /// <returns> The dot product. </returns>
        float Dot(size_t size, const float* pFirst, size_t firstStride, const float* pSecond, size_t secondStride);

        /// <summary> Wraps the BLAS DOT function, which computes the dot product of two vectors. </summary>
        ///
        /// <param name="size"> The size of the vectors. </param>
        /// <param name="pFirst"> Pointer to the first element of the first array. </param>
        /// <param name="firstStride"> The stride of the first array. </param>
        /// <param name="pSecond"> Pointer to the first element of the second array. </param>
        /// <param name="secondStride"> The stride of the second array. </param>
        /// <returns> The dot product. </returns>
        double Dot(size_t size, const double* pFirst, size_t firstStride, const double* pSecond, size_t secondStride);
    }

}