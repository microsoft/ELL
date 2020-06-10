////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Scalar.h"
#include "Value.h"
#include "Vector.h"

#include <utilities/include/MemoryLayout.h>

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

    Scalar Sum(Vector input);

    /// todo pending for PR 1352 to merge
    /// <summary> Turn whatever the data memory layout is into a flat vector </summary>
    Vector ToVector(Value data);

    /// <summary> Creates a for loop over the vector </summary>
    /// <param name="vector"> The instance of Vector that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Vector vector, std::function<void(Scalar)> fn);

    /// <summary> Creates a for loop over the vector </summary>
    /// <param name="name"> A name that can be used by the emitter context to tag this loop in the emitted code </param>
    /// <param name="vector"> The instance of Vector that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(const std::string& name, Vector vector, std::function<void(Scalar)> fn);

} // namespace value
} // namespace ell
