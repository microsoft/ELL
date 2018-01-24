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

        ParametricReLUActivation() = default;

        /// <summary> Instantiates an instance of this class with the learned alpha parameters. </summary>
        ///
        /// <param name="alpha"> The learned alpha parameters. </param>
        ParametricReLUActivation(TensorType alpha);

        /// <summary> Returns the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="index"> The input index. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType Apply(const ElementType input, const math::IntegerTriplet& index) const;

        /// <summary> Typename used for serialization. </summary>
        /// Note: In the future, this will change to include the templated element type
        static std::string GetTypeName() { return "ParametricReLUActivation"; }

        /// <summary> Gets the learned alpha parameters. </summary>
        ///
        /// <returns> The learned alpha parameters. </returns>
        const TensorType& GetAlpha() const { return _alpha; }

        /// <summary> Archives this object. </summary>
        ///
        /// <param name="archiver"> The archiver. </param>
        void WriteToArchive(utilities::Archiver& archiver) const;

        /// <summary> Unarchives this object. </summary>
        ///
        /// <param name="archiver"> The unarchiver. </param>
        void ReadFromArchive(utilities::Unarchiver& archiver);

    private:
        TensorType _alpha;
    };
}
}
}

#include "../tcc/ParametricReLUActivation.tcc"
