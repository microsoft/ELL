////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueVectorOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "MemoryLayout.h"

// stl
#include <functional>

namespace ell
{
namespace value
{

    class Vector;
    class Scalar;

    Scalar Norm0(Vector);

    Scalar Norm1(Vector);

    Scalar Norm2(Vector);

    Scalar Norm2Squared(Vector);

    Scalar Aggregate(Vector, std::function<Scalar(Scalar)>);

    Vector Transform(Vector, std::function<Scalar(Scalar)>);

    Scalar Dot(Vector, Vector);

    /// <summary> Creates a for loop over the vector </summary>
    /// <param name="vector"> The instance of Vector that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Vector vector, std::function<void(Scalar)> fn);

} // namespace value
} // namespace ell