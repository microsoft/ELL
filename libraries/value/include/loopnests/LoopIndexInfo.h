////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopIndexInfo.h (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Index.h"
#include "Range.h"

#include "../Scalar.h"

#include <unordered_map>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        enum class LoopIndexState
        {
            notVisited,
            inProgress,
            done
        };

        struct LoopIndexSymbolTableEntry
        {
            Index scope; // redundant with key in symbol table map
            Scalar value;
            Range loopRange;
            LoopIndexState state;
        };
        using LoopIndexSymbolTable = std::unordered_map<Index, LoopIndexSymbolTableEntry>;
    } // namespace loopnests
} // namespace value
} // namespace ell
