////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquareLoss.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Vector.h"

// stl
#include <type_traits>

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename T>
    using IsNumerical = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type;

    /// <summary> Implements the square-loss function: loss(prediction, output) = 0.5 * (prediction-output) * (prediction-output) </summary>
    class SquareLoss
    {
    public:
        /// <summary> Gets the smoothness of this loss, which is the Lipschitz coefficient of the loss gradient. </summary>
        ///
        /// <returns> The smoothness coefficient. </returns>
        constexpr static double Smoothness() { return 1.0; }

        /// <summary> Returns the loss of a scalar prediction, given the true scalar output. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        ///
        /// <returns> The loss. </returns>
        template <typename OutputType>
        static double Value(double prediction, OutputType output);

        /// <summary> Returns the loss derivative at a given scalar point. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        ///
        /// <returns> The loss derivative. </returns>
        template <typename OutputType>
        static double Derivative(double prediction, OutputType output);

        /// <summary> Returns the value of the loss conjugate at a given point. </summary>
        ///
        /// <param name="dual"> The dual variable. </param>
        /// <param name="output"> The output. </param>
        ///
        /// <returns> Value of the loss conjugate. </returns>
        template <typename OutputType>
        static double Conjugate(double dual, OutputType output);

        /// <summary>
        /// Returns the value of the conjugate prox function at a given point, which is argmin_b
        /// {sigma*(f*)(b) + (1/2)*(b - a)_2^2}.
        /// </summary>
        ///
        /// <param name="sigma"> The sigma parameter. </param>
        /// <param name="dual"> The dual variable. </param>
        /// <param name="output"> The output. </param>
        ///
        /// <returns> Value of the loss conjugate prox. </returns>
        template <typename OutputType>
        static double ConjugateProx(double sigma, double dual, OutputType output);
    };
}
}
}

#include "../tcc/SquareLoss.tcc"
