////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueOperations.h"
#include "EmitterContext.h"
#include "ValueScalar.h"

namespace ell
{
namespace value
{
    using namespace utilities;

    void For(MemoryLayout layout, std::function<void(Scalar)> fn)
    {
        GetContext().For(layout, [&layout, fn = std::move(fn)](std::vector<Scalar> coords) {
            fn(detail::CalculateOffset(layout, coords));
        });
    }

    Value Cast(Value value, ValueType type) { return GetContext().Cast(value, type); }

} // namespace value
} // namespace ell