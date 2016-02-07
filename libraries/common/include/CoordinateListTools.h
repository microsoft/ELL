// CoordinateListFactory.h

#pragma once

#include "MapLoadArguments.h"

// layers
#include "Coordinate.h"
#include "Map.h"

namespace common
{
    /// TODO
    ///
    layers::CoordinateList GetCoordinateList(const layers::Map& map, const CoordinateListLoadArguments& coordinateListLoadArguments);

    /// TODO
    ///
    layers::CoordinateList CoordinateSequence(uint64 row, uint64 numColumns);

    /// TODO
    ///
    layers::CoordinateList CoordinateListIgnoreMapSuffix(const layers::Map& map, uint64 numLayersToDrop);
}
