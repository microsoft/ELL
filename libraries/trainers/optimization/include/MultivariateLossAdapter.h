////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultivariateLossAdapter.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Vector.h"

namespace ell
{
namespace trainers
{
namespace optimization
{
    /// <summary> Adapter that extends a scalar loss class to vector inputs. </summary>
    template <typename LossType>
    class MultivariateLossAdapter : public LossType
    {
    public:
        using LossType::LossType;

        /// <summary> Returns the loss of a vector prediction, given the true vector output. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        ///
        /// <returns> The loss. </returns>
        template <typename OutputElementType>
        double Value(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary> Returns the loss gradient at a given vector point. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        ///
        /// <returns> The loss gradient. </returns>
        template <typename OutputElementType>
        math::RowVector<double> Derivative(math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary> Returns the value of the loss conjugate at a given vector point. </summary>
        ///
        /// <param name="dual"> The dual variable. </param>
        /// <param name="output"> The output. </param>
        ///
        /// <returns> Value of the loss conjugate. </returns>
        template <typename OutputElementType>
        double Conjugate(math::ConstRowVectorReference<double> dual, math::ConstRowVectorReference<OutputElementType> output) const;

        /// <summary>
        /// Returns the value of the conjugate prox function at a given vector point, which is argmin_b
        /// {sigma*(f*)(b) + (1/2)*(b - a)_2^2}.
        /// </summary>
        ///
        /// <param name="sigma"> The sigma parameter. </param>
        /// <param name="prediction"> The prediction. </param>
        /// <param name="output"> The output. </param>
        ///
        /// <returns> Value of the loss conjugate prox. </returns>
        template <typename OutputElementType>
        math::RowVector<double> ConjugateProx(double sigma, math::ConstRowVectorReference<double> prediction, math::ConstRowVectorReference<OutputElementType> output) const;
    };
}
}
}

#include "../tcc/MultivariateLossAdapter.tcc"
