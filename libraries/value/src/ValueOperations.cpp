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

void For(Value value, std::function<void(Scalar)> fn)
{
  if (!value.IsConstrained()) { throw InputException(InputExceptionErrors::invalidArgument); }

    For(value, value.GetLayout(), fn);
}

void For(Value value, MemoryLayout layout, std::function<void(Scalar)> fn)
{
  GetContext().For(value, layout,
        [&layout, &fn](MemoryCoordinates coords)
        {
            fn(static_cast<int>(layout.GetEntryOffset(coords)));
        }
    );
}

}
}