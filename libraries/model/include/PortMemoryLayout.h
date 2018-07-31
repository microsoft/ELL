////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "IRLocalScalar.h"

// utilities
#include "MemoryLayout.h"

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
}

namespace model
{
    using DimensionOrder = utilities::DimensionOrder;
    using MemoryShape = utilities::MemoryShape;
    using MemoryCoordinates = utilities::MemoryCoordinates;
    using PortMemoryLayout = utilities::MemoryLayout;

    /// <summary> Emits LLVM IR to get the offset into memory for an entry </summary>
    ///
    /// <param name="function"> The function to emit code into </param>
    /// <param name="location"> The coordinates of the entry </param>
    /// <param name="layout"> The layout of the memory </param>
    /// <returns> The offset to the entry (from the beginning of memory) </returns>
    emitters::IRLocalScalar EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::vector<emitters::IRLocalScalar>& location, const PortMemoryLayout& layout);

    /// <summary> Emits LLVM IR to check if a location is outside of the stored memory extent in any dimension </summary>
    ///
    /// <param name="function"> The function to emit code into </param>
    /// <param name="location"> The coordinates of the entry </param>
    /// <param name="layout"> The layout of the memory </param>
    /// <returns> A value representing `true` if the location is out of bounds </returns>
    emitters::IRLocalScalar EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::vector<emitters::IRLocalScalar>& location, const PortMemoryLayout& layout);
}
}