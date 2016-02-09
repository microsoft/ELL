////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Map.h"
#include "Scale.h"
#include "Shift.h"
#include "Input.h"
#include "Sum.h"

// stl
#include <stdexcept>
#include <string>

namespace layers
{
    bool Map::Iterator::IsValid() const
    {
        return _index < _outputCoordinates.size();
    }

    void Map::Iterator::Next()
    {
        ++_index;
    }

    /// \returns The current index-value pair
    ///
    IndexValue Map::Iterator::Get() const
    {
        auto coordinate = _outputCoordinates[_index];
        uint64 row = coordinate.GetRow();
        uint64 column = coordinate.GetColumn();
        return IndexValue{ _index, (*_spOutputs)[row][column] };
    }

    Map::Iterator::Iterator(std::shared_ptr<std::vector<types::DoubleArray>> spOutput, const CoordinateList& outputCoordinates) :
        _spOutputs(spOutput),
        _outputCoordinates(outputCoordinates),
        _index(0)
    {}

    Map::Map(uint64 inputLayerSize)
    {
        _layers.push_back(std::make_shared<Input>(inputLayerSize));
    }

    uint64 Map::PushBack(std::shared_ptr<Layer> layer)
    {
        uint64 row = _layers.size();
        _layers.push_back(layer);
        return row;
    }

    uint64 Map::NumLayers() const
    {
        return _layers.size();
    }

    CoordinateList Map::GetCoordinateList(uint64 layerIndex) const
    {
        CoordinateList coordinateList;
        uint64 numColumns = _layers[layerIndex]->Size();
        for (uint64 column = 0; column < numColumns; ++column)
        {
            coordinateList.emplace_back(layerIndex, column);
        }
        return coordinateList;
    }

    void Map::Serialize(utilities::JsonSerializer & serializer) const
    {
        serializer.Write("layers", _layers);
    }

    void Map::Serialize(ostream& os) const
    {
        utilities::JsonSerializer writer;
        writer.Write("Base", *this);
        auto str = writer.ToString();
        os << str;
    }

    void Map::Deserialize(utilities::JsonSerializer & serializer)
    {
        serializer.Read("layers", _layers, DeserializeLayers);
    }

    void Map::DeserializeLayers(utilities::JsonSerializer & serializer, std::shared_ptr<Layer>& up)
    {
        auto type = serializer.Read<std::string>("_type");
        auto version = serializer.Read<int>("_version");

        if (type == "Input")
        {
            up = std::make_shared<Input>();
        }
        else if (type == "Scale")
        {
            up = std::make_shared<Scale>();
        }
        else if (type == "Shift")
        {
            up = std::make_shared<Shift>();
        }
        else if (type == "Sum")
        {
            up = std::make_shared<Sum>();
        }
        else
        {
            throw std::runtime_error("unidentified type in map file: " + type);
        }

        up->Deserialize(serializer, version);
    }

    std::shared_ptr<std::vector<types::DoubleArray>> Map::AllocateOutputs() const
    {
        auto outputs = std::make_shared<std::vector<types::DoubleArray>>();
        for (uint64 i = 0; i < _layers.size(); ++i)
        {
            outputs->emplace_back(_layers[i]->Size());
        }
        return outputs;
    }
}
