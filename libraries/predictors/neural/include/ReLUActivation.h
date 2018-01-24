////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.h (neural)
//  Authors:  Byron Changuion
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
    /// <summary> Implements the ReLU function: loss(x) = (x > 0) ? x : 0  </summary>
    template <typename ElementType>
    class ReLUActivation
    {
    public:
        /// <summary> Returns the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType operator()(const ElementType input) const;

        /// <summary> Returns the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType Apply(const ElementType input) const;

        /// <summary> Returns the output as a function of the input. </summary>
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
        static std::string GetTypeName() { return "ReLUActivation"; }

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

#include "../tcc/ReLUActivation.tcc"
