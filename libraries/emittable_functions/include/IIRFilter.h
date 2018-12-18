////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilter.h (emittable_functions)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/Vector.h>

namespace ell
{
namespace emittable_functions
{

    struct IIRFilterCoefficients
    {
        value::Vector b; // feedforward coeffs
        value::Vector a; // feedback coeffs
    };

    value::Vector FilterSamples(value::Vector signal, IIRFilterCoefficients filterCoeffs);

} // namespace emittable_functions
} // namespace ell
