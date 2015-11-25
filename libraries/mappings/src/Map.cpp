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

    Map::Iterator Map::GetIterator(uint64 layerIndex) const
    {
        if (layerIndex >= _layers.size())
        {
            layerIndex = _layers.size() - 1;
        }

        return _layers[layerIndex]->GetIterator();
    }
}
