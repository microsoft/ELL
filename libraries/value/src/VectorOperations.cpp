////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VectorOperations.h"
#include "EmitterContext.h"
#include "Vector.h"

namespace ell
{
using namespace utilities;

namespace value
{

void For(Vector v, std::function<void(Scalar)> fn)
{
    For(v, static_cast<Value>(v).GetLayout(), fn);
}

void For(Vector v, MemoryLayout layout, std::function<void(Scalar)> fn)
{
    if (layout.NumDimensions() != 1)
    {
        throw InputException(InputExceptionErrors::invalidArgument, "Layout being looped over must be one-dimensional");
    }

    GetContext().For(static_cast<Value>(v), layout,
    [fn = std::move(fn)](MemoryCoordinates coordinates)
    {
        fn(coordinates[0]);
    });
}

}
}