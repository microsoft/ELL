////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueMatrixOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ValueScalar.h"

// utilities
#include "MemoryLayout.h"

// stl
#include <functional>

namespace ell
{
namespace value
{

    class Matrix;

    /// <summary> Creates a for loop over the matrix </summary>
    /// <param name="matrix"> The instance of Matrix that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Matrix matrix, std::function<void(Scalar, Scalar)> fn);

} // namespace value
} // namespace ell