// CoordinateListFactory.h

#pragma once

#include "Coordinate.h"
#include "Map.h"

#include <vector>
using std::vector;

namespace layers
{
    class CoordinateListFactory
    {
    public:
        static CoordinateList Sequence(uint64 row, uint64 numColumns, uint64 fromColumn = 0);
        static CoordinateList IgnoreSuffix(const Map& map, uint64 numLayersToDrop);

    private:
        static void FillBack(CoordinateList& vector, uint64 row, uint64 numColumns, uint64 fromColumn = 0);
    };
}
