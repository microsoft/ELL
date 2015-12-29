// Map.cpp

#include "Map.h"
#include "layers.h"

using std::make_shared;
using std::move; // TODO remove?

namespace layers
{
    bool Map::Iterator::IsValid() const
    {
        return _index < _spOutputCoordinates->size();
    }

    void Map::Iterator::Next()
    {
        ++_index;
    }

    /// \returns The current index-value pair
    ///
    IndexValue Map::Iterator::Get() const
    {
        auto coordinate = (*_spOutputCoordinates)[_index];
        uint64 row = coordinate.GetRow();
        uint64 column = coordinate.GetColumn();
        return IndexValue{ _index, (*_spOutputs)[row][column] };
    }

    Map::Iterator::Iterator(shared_ptr<vector<vector<double>>> spOutput, shared_ptr<vector<Coordinate>> spOutputCoordinates) :
        _spOutputs(spOutput),
        _spOutputCoordinates(spOutputCoordinates),
        _index(0)
    {}

    Map::Map(uint64 inputLayerSize)
    {
        _layers.push_back(make_shared<Input>(inputLayerSize));
    }

    uint64 Map::PushBack(shared_ptr<Layer> layer)
    {
        uint64 row = _layers.size();
        _layers.push_back(layer);
        return row;
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

    shared_ptr<vector<vector<double>>> Map::AllocateOutputs() const
    {
        auto outputs = make_shared<vector<vector<double>>>();
        for (uint64 i = 0; i < _layers.size(); ++i)
        {
            outputs->emplace_back(_layers[i]->Size());
        }
        return outputs;
    }
}
