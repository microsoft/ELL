////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2Regularizer.h (functions)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Vector.h"

namespace ell
{
namespace functions
{
    /// <summary> Implements a squared L2 norm regularizer. </summary>
    class L2Regularizer
    {
    public:
        /// <summary> Computes the value of the regularizer at a given point. </summary>
        ///
        /// <param name="w"> The point at which the regularizer is computed. </param>
        /// <param name="b"> (Optional) The bias term for which the regularizer is computed. </param>
        ///
        /// <returns> Value of the regularizer. </returns>
        double operator()(math::ConstColumnVectorReference<double> w, double b=0) const;

        /// <summary> Computes the value of the convex conjugate of the regularizer. </summary>
        ///
        /// <param name="v"> The point at which the conjugate is computed. </param>
        /// <param name="d"> (Optional) The bias term for which the conjugate is computed. </param>
        /// <returns> Value of the conjugate. </returns>
        double Conjugate(math::ConstColumnVectorReference<double> v, double d=0) const;

        /// <summary> Computes the conjugate gradient function. Namely, Given vector v, 
        /// compute w = argmax_u {v'*u - f(u)} = argmin_u {-v'*u + f(u)} </summary>
        ///
        /// <param name="v"> The point at which the conjugate gradient is computed. </param>
        /// <param name="w"> The output. </param>
        void ConjugateGradient(math::ConstColumnVectorReference<double> v, math::ColumnVectorReference<double> w) const;

        /// <summary>
        /// Computes the conjugate gradient function. Namely, Given vector v, compute g = argmax_w {v'*w - f(w)} = argmin_w {-v'*w + f(w)}
        /// </summary>
        ///
        /// <param name="v"> The vector at which the conjugate is computed. </param>
        /// <param name="d"> The bias term for which the conjugate is computed. </param>
        /// <param name="w"> The output vector. </param>
        /// <param name="b"> [in,out] The output bias term. </param>
        void ConjugateGradient(math::ConstColumnVectorReference<double> v, double d, math::ColumnVectorReference<double> w, double& b) const;
    };
}
}