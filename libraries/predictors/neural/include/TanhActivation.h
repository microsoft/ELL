////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TanhActivation.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"
#include "Vector.h"

// stl
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the hyperbolic tangent function: tanh(x) = 2 . sigmoid(2x) - 1 </summary>
    template <typename ElementType>
    class TanhActivation
    {
    public:
        /// <summary> Returns the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType operator()(const ElementType input) const;

        /// <summary> Returns the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType Apply(const ElementType input) const;

        /// <summary> Returns the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="index"> The input index. </param>
        ElementType Apply(const ElementType input, const math::IntegerTriplet& index) const;

        /// <summary> Applies the activation to the input vector in-place. </summary>
        ///
        /// <param name="input"> The input vector. </param>
        void Apply(math::ColumnVectorReference<ElementType>& input) const;

        /// <summary> Typename used for serialization. </summary>
        /// Note: In the future, this will change to include the templated element type
        static std::string GetTypeName() { return "TanhActivation"; }

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

#include "../tcc/TanhActivation.tcc"
