////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasWrapper.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define USE_BLAS

#include <cstddef> // size_t

namespace math
{
    namespace Blas
    {
        /// <summary> Wraps the BLAS ASUM function, which computes the 1-norm of a vector . </summary>
        ///
        /// <param name="size"> The size of the array that stores the vector. </param>
        /// <param name="ptr"> Pointer to the first element of the array. </param>
        /// <param name="Increment"> The Increment of the array. </param>
        ///
        /// <returns> The 1-norm of the vector. </returns>
        float Asum(size_t size, const float* ptr, size_t Increment);

        /// <summary> Wraps the BLAS ASUM function, which computes the 1-norm of a vector . </summary>
        ///
        /// <param name="size"> The size of the array that stores the vector. </param>
        /// <param name="ptr"> Pointer to the first element of the array. </param>
        /// <param name="Increment"> The Increment of the array. </param>
        ///
        /// <returns> The 1-norm of the vector. </returns>
        double Asum(size_t size, double* ptr, size_t Increment);

        /// <summary> Wraps the BLAS NRM2 function, which computes the 2-norm of a vector . </summary>
        ///
        /// <param name="size"> The size of the array that stores the vector. </param>
        /// <param name="ptr"> Pointer to the first element of the array. </param>
        /// <param name="Increment"> The Increment of the array. </param>
        ///
        /// <returns> The 2-norm of the vector. </returns>
        float Nrm2(size_t size, const float* ptr, size_t Increment);

        /// <summary> Wraps the BLAS NRM2 function, which computes the 2-norm of a vector . </summary>
        ///
        /// <param name="size"> The size of the array that stores the vector. </param>
        /// <param name="ptr"> Pointer to the first element of the array. </param>
        /// <param name="Increment"> The Increment of the array. </param>
        ///
        /// <returns> The 2-norm of the vector. </returns>
        double Nrm2(size_t size, double* ptr, size_t Increment);

        /// <summary> Wraps the BLAS SCAL function, which multiplies a vector by a scalar. </summary>
        ///
        /// <param name="size"> The size of the array that stores the vector. </param>
        /// <param name="alpha"> The scalar that multiplies the vector. </param>
        /// <param name="pLhs"> [in,out] Pointer to the first element of the array. </param>
        /// <param name="lhsIncrement"> The Increment of the array. </param>
        void Scal(size_t size, float alpha, float* pLhs, size_t lhsIncrement);
        
        /// <summary> Wraps the BLAS SCAL function, which multiplies a vector by a scalar. </summary>
        ///
        /// <param name="size"> The size of the array that stores the vector. </param>
        /// <param name="alpha"> The scalar that multiplies the vector. </param>
        /// <param name="pLhs"> [in,out] Pointer to the first element of the array. </param>
        /// <param name="lhsIncrement"> The Increment of the array. </param>
        void Scal(size_t size, double alpha, double* pLhs, size_t lhsIncrement);

        /// <summary> Wraps the BLAS AXPY function: v += alpha * u </summary>
        ///
        /// <param name="size"> The size of each of the arrays that store the vectors. </param>
        /// <param name="alpha"> The scalar that multiplies the right-hand-side array. </param>
        /// <param name="pRhs"> Pointer to the first element of the right-hand-side array. </param>
        /// <param name="rhsIncrement"> The Increment of the right-hand-side array. </param>
        /// <param name="pLhs"> [in,out] Pointer to the first element of the left-hand-side array, which is modified by this procedure. </param>
        /// <param name="lhsIncrement"> The Increment of the left-hand-side array. </param>
        void Axpy(size_t size, float alpha, const float* pRhs, size_t rhsIncrement, float* pLhs, size_t lhsIncrement);

        /// <summary> Wraps the BLAS AXPY function: v += alpha * u </summary>
        ///
        /// <param name="size"> The size of each of the arrays that store the vectors. </param>
        /// <param name="alpha"> The scalar that multiplies the right-hand-side array. </param>
        /// <param name="pRhs"> Pointer to the first element of the right-hand-side array. </param>
        /// <param name="rhsIncrement"> The Increment of the right-hand-side array. </param>
        /// <param name="pLhs"> [in,out] The left-hand-side array, which is modified by this procedure. </param>
        /// <param name="lhsIncrement"> The Increment of the left-hand-side array. </param>
        void Axpy(size_t size, double alpha, const double* pRhs, size_t rhsIncrement, double* pLhs, size_t lhsIncrement);

        /// <summary> Wraps the BLAS DOT function, which computes the dot product of two vectors. </summary>
        ///
        /// <param name="size"> The size of each of the arrays that store the vectors. </param>
        /// <param name="pFirst"> Pointer to the first element of the first array. </param>
        /// <param name="firstIncrement"> The Increment of the first array. </param>
        /// <param name="pSecond"> Pointer to the first element of the second array. </param>
        /// <param name="secondIncrement"> The Increment of the second array. </param>
        /// <returns> The dot product. </returns>
        float Dot(size_t size, const float* pFirst, size_t firstIncrement, const float* pSecond, size_t secondIncrement);

        /// <summary> Wraps the BLAS DOT function, which computes the dot product of two vectors. </summary>
        ///
        /// <param name="size"> The size of each of the arrays that store the vectors. </param>
        /// <param name="pFirst"> Pointer to the first element of the first array. </param>
        /// <param name="firstIncrement"> The Increment of the first array. </param>
        /// <param name="pSecond"> Pointer to the first element of the second array. </param>
        /// <param name="secondIncrement"> The Increment of the second array. </param>
        /// <returns> The dot product. </returns>
        double Dot(size_t size, const double* pFirst, size_t firstIncrement, const double* pSecond, size_t secondIncrement);


        // gemm
        // gemv


        // acpby
        // Increment_dot
    }

}