////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParametricReLUActivation.h (neural)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the parametric ReLU function: activation(x, alpha, i) = (x[i] > 0) ? x[i] : alpha[i] * x[i] </summary>
    template <typename ElementType>
    class ParametricReLUActivation
    {
    public:
        using ConstTensorReferenceType = math::ConstTensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using TensorType = math::Tensor<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;

        /// <summary> Instantiates an instance of this class with the learned alpha parameters. </summary>
        ///
        /// <param name="alpha"> The learned alpha parameters. </param>
        ParametricReLUActivation(ConstTensorReferenceType alpha)
            : _alpha(alpha)
        {
        }

        ParametricReLUActivation(const ParametricReLUActivation& other) = default;

        /// <summary> Sets the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="index"> The input index. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType Apply(const ElementType input, const math::Triplet& index) const;

        /// <summary> Typename used for serialization. </summary>
        static std::string GetTypeName() { return "ParametricReLUActivation"; }

    private:
        TensorType _alpha;
    };
}
}
}

#include "../tcc/ParametricReLUActivation.tcc"