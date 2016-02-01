// CoordinateListTools.h

#pragma once

#include "MapLoadArguments.h"

// layers
#include "Coordinate.h"
using layers::CoordinateList;

#include "Map.h"
using layers::Map;

namespace common
{
    CoordinateList GetCoordinateList(const Map& map, const CoordinateListLoadArguments& coordinateListLoadArguments);

    CoordinateList CoordinateSequence(uint64 row, uint64 numColumns);

    CoordinateList CoordinateListIgnoreMapSuffix(const Map& map, uint64 numLayersToDrop);
}
