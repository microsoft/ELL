////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerOptions.h (model)
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
        none = -1,
        unrolled = 0,
        diagonal,
        simple,
        winograd,
    };

    struct ModelOptimizerOptions
    {
        // individual optimization settings
        bool fuseLinearFunctionNodes = true;

        PreferredConvolutionMethod preferredConvolutionMethod = PreferredConvolutionMethod::none;
    };
}
}
