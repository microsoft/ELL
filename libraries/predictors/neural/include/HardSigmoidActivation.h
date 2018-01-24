////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HardSigmoidActivation.h (neural)
//  Authors:  James Devine
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
    /// <summary> Implements the "hard sigmoid" function: loss(x) = clip(0.2x + 0.5, 0, 1) (where clip(y, a,b) == max(min(y,b),a)) </summary>
    template <typename ElementType>
    class HardSigmoidActivation
    {
    public:
        /// <summary> Applies the activation function to a value. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType operator()(const ElementType input) const;

        /// <summary> Applies the activation function to a value. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType Apply(const ElementType input) const;

        /// <summary> Applies the activation function to a value. </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="index"> The input index. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType Apply(const ElementType input, const math::IntegerTriplet& index) const;

        /// <summary> Applies the activation to the input vector in-place. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        void Apply(math::ColumnVector<ElementType>& input) const;

        /// <summary> Typename used for serialization. </summary>
        /// Note: In the future, this will change to include the templated element type
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("HardSigmoidActivation"); }

        /// <summary> Archives this object. </summary>
        ///
        /// <param name="archiver"> The archiver. </param>
        void WriteToArchive(utilities::Archiver& /*archiver*/) const {};

        /// <summary> Unarchives this object. </summary>
        ///
        /// <param name="archiver"> The unarchiver. </param>
        void ReadFromArchive(utilities::Unarchiver& /*archiver*/) {};
    };
}
}
}

#include "../tcc/HardSigmoidActivation.tcc"
