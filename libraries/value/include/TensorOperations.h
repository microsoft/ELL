////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Scalar.h"

#include <utilities/include/MemoryLayout.h>

#include <functional>

namespace ell
{
namespace value
{

    class Tensor;

    enum class Slice
    {
        All
    };

    Scalar Sum(Tensor tensor);

    /// <summary> Creates a for loop over the tensor </summary>
    /// <param name="tensor"> The instance of Tensor that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Tensor tensor, std::function<void(Scalar, Scalar, Scalar)> fn);

    /// <summary> Creates a for loop over the tensor </summary>
    /// <param name="name"> A name that can be used by the emitter context to tag this loop in the emitted code </param>
    /// <param name="tensor"> The instance of Tensor that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(const std::string& name, Tensor tensor, std::function<void(Scalar, Scalar, Scalar)> fn);

} // namespace value
} // namespace ell
