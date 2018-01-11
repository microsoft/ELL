////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MathConstants.h (math)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace math
{
    /// <summary> A struct that holds various mathematical constants. </summary>
    template <typename ValueType>
    struct Constants
    {
        constexpr static const ValueType pi = static_cast<ValueType>(3.14159265358979323846);
    };
}
}
