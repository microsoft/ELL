////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "MemoryLayout.h"

// llvm
#include <llvm/IR/Value.h>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
}

namespace model
{
    using MemoryShape = utilities::MemoryShape;
    using PortMemoryLayout = utilities::MemoryLayout;

    /// <summary> Emits LLVM IR to get the offset into memory for an entry </summary>
    ///
    /// <param name="function"> The function to emit code into </param>
    /// <param name="location"> The coordinates of the entry </param>
    /// <param name="layout"> The layout of the memory </param>
    /// <returns> The offset to the entry (from the beginning of memory) </returns>
    llvm::Value* EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::vector<llvm::Value*>& location, const PortMemoryLayout& layout);

    /// <summary> Emits LLVM IR to check if a location is outside of the stored memory extent in any dimension </summary>
    ///
    /// <param name="function"> The function to emit code into </param>
    /// <param name="location"> The coordinates of the entry </param>
    /// <param name="layout"> The layout of the memory </param>
    /// <returns> A value representing `true` if the location is out of bounds </returns>
    llvm::Value* EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::vector<llvm::Value*>& location, const PortMemoryLayout& layout);
}
}