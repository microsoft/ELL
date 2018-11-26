////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ValueType.h"

#include <utilities/include/Boolean.h>
#include <utilities/include/MemoryLayout.h>

#include <functional>

namespace ell
{
namespace value
{
    class Value;
    class Scalar;

    /// <summary> Creates a for loop over the given layout </summary>
    /// <param name="layout"> The layout used to describe the iteration characteristics. Only active elements are iterated over. </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(utilities::MemoryLayout layout, std::function<void(Scalar)> fn);

    /// <summary> Cast a value to another type, returning a new value </summary>
    /// <param name="value"> The data to convert </param>
    /// <param name="type"> The type to which the data should be casted </param>
    /// <returns> A new value instance with the casted data </returns>
    /// <remarks> The new Value instance's data is distinct from the original </remarks>
    Value Cast(Value value, ValueType type);

    // Defined in Value.tcc
    /// <summary> Cast a value to another type, returning a new value </summary>
    /// <typeparam name="T"> The type to which the data should be casted </typeparam>
    /// <param name="value"> The data to convert </param>
    /// <returns> A new value instance with the casted data </returns>
    /// <remarks> The new Value instance's data is distinct from the original </remarks>
    template <typename T>
    Value Cast(Value value);

    template <typename CastType, typename ViewType>
    ViewType Cast(ViewType value)
    {
        return Cast<CastType>(value.GetValue());
    }

} // namespace value
} // namespace ell
