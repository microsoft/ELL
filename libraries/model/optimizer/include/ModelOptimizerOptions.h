////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerOptions.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace model
{
    enum class PreferredConvolutionMethod : int
    {
        automatic = 0,
        diagonal,
        simple,
        winograd,
        unrolled
    };

    struct ModelOptimizerOptions
    {
        // individual optimization settings
        bool fuseLinearFunctionNodes = true;
        bool optimizeReorderDataNodes = true;

        PreferredConvolutionMethod preferredConvolutionMethod = PreferredConvolutionMethod::automatic;
    };
} // namespace model
} // namespace ell
