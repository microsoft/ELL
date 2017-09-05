////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorShape.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// stl
#include <cstring> // for size_t

namespace ell
{
namespace math
{
    /// <summary> A handy struct that describes the shape of a tensor. </summary>
    struct TensorShape
    {
        size_t rows;
        size_t columns;
        size_t channels;

    /// <summary> Gets the number of elements. </summary>
    ///
    /// <returns> The number of elements in the `TensorShape` </returns>
    size_t Size() const { return rows * columns * channels; }
    };
}
}