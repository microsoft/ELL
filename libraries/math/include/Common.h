////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Common.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace math
{
    /// <summary> An enum that represent different implementation types. </summary>
    enum class ImplementationType
    {
        native,
        openBlas
    };

    /// <summary> A stub class that represents the scalar one. </summary>
    struct One {};
}
}