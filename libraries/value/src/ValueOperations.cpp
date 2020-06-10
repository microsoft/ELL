////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueOperations.h"
#include "EmitterContext.h"
#include "Scalar.h"

namespace ell
{
namespace value
{
    using namespace utilities;

    void ForSequence(Scalar end, std::function<void(Scalar)> fn)
    {
        throw LogicException(LogicExceptionErrors::notImplemented);
    }

    void For(MemoryLayout layout, std::function<void(Scalar)> fn)
    {
        GetContext().For(layout, [&layout, fn = std::move(fn)](std::vector<Scalar> coords) {
            fn(detail::CalculateOffset(layout, coords));
        });
    }

    void For(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn)
    {
        GetContext().For(start, stop, step, fn);
    }

    Value Cast(Value value, ValueType type)
    {
        if (value.GetBaseType() == type)
        {
            return value;
        }

        return GetContext().Cast(value, type);
    }

} // namespace value
} // namespace ell
