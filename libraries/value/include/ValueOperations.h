////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueOperations.h (value)
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
class Value;
class Scalar;

/// <summary> Creates a for loop over the memory pointed to by the Value instance </summary>
/// <param name="value"> The instance of Value that references the data over which to iterate </param>
/// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
void For(Value value, std::function<void(Scalar)> fn);

/// <summary> Creates a for loop over the memory pointed to with the given layout </summary>
/// <param name="value"> The instance of Value that references the data over which to iterate </param>
/// <param name="layout"> The layout used to describe the iteration characteristics. Only active elements are iterated over. </param>
/// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
void For(Value value, utilities::MemoryLayout layout, std::function<void(Scalar)> fn);

}
}
