////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>

namespace ell
{
namespace nodes
{

    /// <summary> A vector of numbers representing shape information. </summary>
    using Shape = std::vector<size_t>;

    /// <summary> Checks if two shapes are equal. </summary>
    ///
    /// <param name="shape1"> The first shape. </param>
    /// <param name="shape2"> The other shape. </param>
    bool ShapesEqual(const Shape& shape1, const Shape& shape2);

    /// <summary> A struct representing the memory layout of port data. </summary>
    struct PortMemoryLayout
    {
        Shape size; // the "active" area of the memory
        Shape stride; // the allocated size along each dimension
        Shape offset; // the offset to the active area for each dimension
    };

    /// <summary> Checks if two memory layouts are equal. </summary>
    ///
    /// <param name="layout1"> The first memory layout. </param>
    /// <param name="layout2"> The other memory layout. </param>
    bool PortMemoryLayoutsEqual(const PortMemoryLayout& layout1, const PortMemoryLayout& layout2);
}
}
