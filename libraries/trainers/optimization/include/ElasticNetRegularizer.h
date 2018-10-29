////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ElasticNetRegularizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VectorSolution.h"
#include "MatrixSolution.h"

namespace ell
{
namespace trainers
{
namespace optimization
{
    /// <summary> Implements a squared L2 norm regularizer. </summary>
    class ElasticNetRegularizer
    {
    public:
        /// <summary> Constructor. </summary>
        ElasticNetRegularizer(double beta = 1.0) : _beta(beta) {}

        /// <summary> Returns the value of the regularizer at a given point. </summary>
        ///
        /// <param name="w"> The point for which the regularizer is computed. </param>
        template <typename SolutionType>
        double Value(const SolutionType& w) const;

        /// <summary> Returns the value of the convex conjugate of the regularizer. </summary>
        ///
        template <typename SolutionType>
        double Conjugate(const SolutionType& v) const;

        /// <summary> Returns the gradient of the conjugate. Namely, Given vector v, 
        /// compute w = argmax_u {v'*u - f(u)} </summary>
        ///
        /// <param name="v"> The point at which the conjugate gradient is computed. </param>
        /// <param name="w"> The output. </param>
        template <typename SolutionType>
        void ConjugateGradient(const SolutionType& v, SolutionType& w) const;

    private:
        double _beta;
    };
}
}
}

#include "../tcc/ElasticNetRegularizer.tcc"