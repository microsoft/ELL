// Map.cpp

#include "Map.h"

using std::make_unique;
using std::move;

namespace mappings
{
    Map::Map(uint64 layerZeroSize)
    {
        _layers.push_back(make_unique<Layer>(layerZeroSize));
    }
}
