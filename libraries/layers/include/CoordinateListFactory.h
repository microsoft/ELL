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
        static vector<Coordinate> Sequence(uint64 row, uint64 numColumns, uint64 fromColumn = 0);
        static vector<Coordinate> IgnoreSuffix(const Map& map, uint64 numLayersToDrop);

    private:
        static void FillBack(vector<Coordinate>& vector, uint64 row, uint64 numColumns, uint64 fromColumn = 0);
    };
}
