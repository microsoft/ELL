// CoordinateListFactory.h

#pragma once

#include "MapLoadArguments.h"

// layers
#include "Coordinate.h"

#include "Map.h"

namespace common
{
    layers::CoordinateList GetCoordinateList(const layers::Map& map, const CoordinateListLoadArguments& coordinateListLoadArguments);

    layers::CoordinateList CoordinateSequence(uint64 row, uint64 numColumns);

    layers::CoordinateList CoordinateListIgnoreMapSuffix(const layers::Map& map, uint64 numLayersToDrop);
}
