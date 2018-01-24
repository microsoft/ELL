////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the leaky ReLU function: activation(x) = (x > 0) ? x : leakyFactor * x  </summary>
    template <typename ElementType>
    class LeakyReLUActivation
    {
    public:
        /// <summary> Instantiates an instance of this class with appropriate leaky factor.
        /// Typical values are 0.1 and 0.01.
        /// </summary>
        ///
        /// <param name="leakyFactor"> The value to multiply the input by if it is less than zero. </param>
        LeakyReLUActivation(ElementType leakyFactor = static_cast<ElementType>(0.1)) : _leakyFactor(leakyFactor) {}

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

        /// <summary> Gets the leaky factor parameter. </summary>
        ///
        /// <returns> The leaky factor parameter. </returns>
        ElementType GetLeakyFactor() const { return _leakyFactor; }

        /// <summary> Typename used for serialization. </summary>
        /// Note: In the future, this will change to include the templated element type
        static std::string GetTypeName() { return "LeakyReLUActivation"; }

        /// <summary> Archives this object. </summary>
        ///
        /// <param name="archiver"> The archiver. </param>
        void WriteToArchive(utilities::Archiver& /*archiver*/) const {};

        /// <summary> Unarchives this object. </summary>
        ///
        /// <param name="archiver"> The unarchiver. </param>
        void ReadFromArchive(utilities::Unarchiver& /*archiver*/) {};

    private:
        ElementType _leakyFactor;
    };
}
}
}

#include "../tcc/LeakyReLUActivation.tcc"
