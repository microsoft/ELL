// MapLoaders.h

#pragma once

// layers
#include "Map.h"
#include "Coordinate.h"

// stl
#include <string>

namespace common
{
    /// Get a map, based on command line parameters
    ///
    template<typename MapType = layers::Map>
    MapType GetMap(const std::string& inputMapFile);
}

#include "../tcc/MapLoaders.tcc"

