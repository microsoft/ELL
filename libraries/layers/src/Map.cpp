// Map.cpp

#include "Map.h"
#include "layers.h"

using std::make_shared;
using std::move;

namespace layers
{
    Map::Map(uint64 inputLayerSize)
    {
        _layers.push_back(make_shared<Input>(inputLayerSize));
    }

    Map::Iterator Map::GetIterator(uint64 layerIndex) const
    {
        if (layerIndex >= _layers.size())
        {
            layerIndex = _layers.size() - 1;
        }

        return _layers[layerIndex]->GetIterator();
    }

    uint64 Map::PushBack(shared_ptr<Layer> layer)
    {
        _layers.push_back(layer);
        return _layers.size() - 1;
    }

    void Map::Serialize(JsonSerializer & serializer) const
    {
        serializer.Write("layers", _layers);
    }

    void Map::Serialize(ostream& os) const
    {
        JsonSerializer writer;
        writer.Write("Base", *this);
        auto str = writer.ToString();
        os << str;
    }

    void Map::Deserialize(JsonSerializer & serializer)
    {
        serializer.Read("layers", _layers, DeserializeLayers);
    }

    void Map::DeserializeLayers(JsonSerializer & serializer, shared_ptr<Layer>& up)
    {
        auto type = serializer.Read<string>("_type");
        auto version = serializer.Read<int>("_version");

        if (type == "Input")
        {
            up = make_shared<Input>();
        }
        else if (type == "Scale")
        {
            up = make_shared<Scale>();
        }
        else if (type == "Shift")
        {
            up = make_shared<Shift>();
        }
        else if (type == "Sum")
        {
            up = make_shared<Sum>();
        }
        else
        {
            throw runtime_error("unidentified type in map file: " + type);
        }

        up->Deserialize(serializer, version);
    }
}
