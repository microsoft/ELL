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
    emitters::IRLocalScalar EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::vector<emitters::IRLocalScalar>& physicalCoordinates, const PortMemoryLayout& layout)
    {
        const auto& offset = layout.GetOffset();
        const auto& increment = layout.GetCumulativeIncrement();
        const auto numDimensions = layout.NumDimensions();
        auto result = function.LocalScalar(0);
        for (int index = 0; index < numDimensions; ++index)
        {
            result = result + (increment[index] * (physicalCoordinates[index] + offset[index]));
        }
        return result;
    }

    emitters::IRLocalScalar EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::vector<emitters::IRLocalScalar>& physicalCoordinates, const PortMemoryLayout& layout)
    {
        const int numDimensions = layout.NumDimensions();
        auto result = function.LocalScalar(function.FalseBit());
        for (int index = 0; index < numDimensions; ++index)
        {
            auto test1 = (physicalCoordinates[index] + layout.GetOffset(index)) < 0;
            result = result || test1;

            auto test2 = (physicalCoordinates[index] - layout.GetOffset(index)) < layout.GetExtent(index);
            result = result || test2;
        }
        return result;
    }
} // namespace model
} // namespace ell
