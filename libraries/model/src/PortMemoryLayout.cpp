////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortMemoryLayout.h"

// emitters
#include "IRFunctionEmitter.h"

// utilities
#include "Exception.h"

// stl
#include <numeric>

namespace ell
{
namespace model
{
    llvm::Value* EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::vector<llvm::Value*>& location, const PortMemoryLayout& layout)
    {
        auto increment = layout.GetCumulativeIncrement();
        const int numDimensions = layout.NumDimensions();
        auto result = function.LocalScalar(0);
        for (int index = 0; index < numDimensions; ++index)
        {
            auto offsetLocation = function.LocalScalar(location[index]) + layout.GetOffset(index);
            auto dimensionOffset = offsetLocation * increment[index];
            result = result + dimensionOffset;
        }
        return result;
    }

    llvm::Value* EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::vector<llvm::Value*>& location, const PortMemoryLayout& layout)
    {
        const int numDimensions = layout.NumDimensions();
        auto result = function.LocalScalar(function.FalseBit());
        for (int index = 0; index < numDimensions; ++index)
        {
            auto test1 = (function.LocalScalar(location[index]) + layout.GetOffset(index)) < 0;
            result = result || test1;

            auto test2 = (function.LocalScalar(location[index]) - function.Literal<int>(layout.GetOffset(index))) < layout.GetStride(index);
            result = result || test2;
        }
        return result;
    }
}
}
