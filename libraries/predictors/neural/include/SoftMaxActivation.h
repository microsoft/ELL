////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxActivation.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"
#include "Vector.h"

// stl
#include <memory>

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the softmax function. </summary>
    template <typename ElementType>
    class SoftMaxActivation
    {
    public:
        /// <summary> Applies the activation to the input vector in-place. </summary>
        ///
        /// <param name="input"> The input value. </param>
        void operator()(math::ColumnVectorReference<ElementType>& input) const;

        /// <summary> Applies the activation to the input vector in-place. </summary>
        ///
        /// <param name="input"> The input value. </param>
        void Apply(math::ColumnVectorReference<ElementType>& input) const;

        /// <summary> Typename used for serialization. </summary>
        /// Note: In the future, this will change to include the templated element type
        static std::string GetTypeName() { return "SoftMaxActivation"; }

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

#include "../tcc/SoftMaxActivation.tcc"
