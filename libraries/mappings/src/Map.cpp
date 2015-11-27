// Map.cpp

#include "Map.h"
#include "Zero.h"
#include "deserializer.h"

using std::make_unique;
using std::move;

namespace mappings
{
    Map::Map(uint64 inputLayerSize)
    {
        _layers.push_back(make_unique<Zero>(inputLayerSize));
    }

    Map::Iterator Map::GetIterator(uint64 layerIndex) const
    {
        if (layerIndex >= _layers.size())
        {
            layerIndex = _layers.size() - 1;
        }

        return _layers[layerIndex]->GetIterator();
    }

    void Map::Add(const SharedLinearBinaryPredictor & predictor, const IndexPairList & predictorInput)
    {
        // TODO - implement this, but first add Sum
    }

    void Map::Serialize(JsonSerializer & serializer) const
    {
        serializer.Write("layers", _layers);
    }

    void Map::Deserialize(JsonSerializer & serializer)
    {
        serializer.Read("layers", _layers);
    }
}
