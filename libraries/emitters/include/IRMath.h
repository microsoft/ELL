////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRValueFunctions.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"
#include "IRFunctionEmitter.h"
#include "IRLocalValue.h"
#include "LLVMUtilities.h"

#include <utilities/include/TypeTraits.h>

namespace ell
{
namespace emitters
{
    // Common math functions
    IRLocalScalar Abs(IRLocalScalar a);
    IRLocalScalar Sqrt(IRLocalScalar a);
    IRLocalScalar Exp(IRLocalScalar a);
    IRLocalScalar Log(IRLocalScalar a);
    IRLocalScalar Sin(IRLocalScalar a);
    IRLocalScalar Cos(IRLocalScalar a);

    template <typename ValueType>
    IRLocalScalar Tanh(IRLocalScalar a);

    IRLocalScalar Min(IRLocalScalar a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Min(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Min(IRLocalScalar a, ValueType b);

    IRLocalScalar Max(IRLocalScalar a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Max(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Max(IRLocalScalar a, ValueType b);
} // namespace emitters
} // namespace ell

#pragma region implementation

namespace ell
{
namespace emitters
{
    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Min(ValueType value, IRLocalScalar b)
    {
        return Min(detail::ToIRLocalScalar(b.function, value), b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Min(IRLocalScalar a, ValueType value)
    {
        return Min(value, a);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Max(ValueType value, IRLocalScalar b)
    {
        return Max(detail::ToIRLocalScalar(b.function, value), b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Max(IRLocalScalar a, ValueType value)
    {
        return Max(value, a);
    }
} // namespace emitters
} // namespace ell

#pragma endregion implementation