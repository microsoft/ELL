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
#include "Coordinatewise.h"
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
        uint64 layerIndex = coordinate.GetLayerIndex();
        uint64 elementIndex = coordinate.GetElementIndex();
        return IndexValue{ _index, (*_spOutputs)[layerIndex][elementIndex] };
    }

    Map::Iterator::Iterator(std::shared_ptr<std::vector<std::vector<double>>> spOutput, const CoordinateList& outputCoordinates) :
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
        uint64 layerIndex = _layers.size();
        _layers.push_back(layer);
        return layerIndex;
    }

    uint64 Map::NumLayers() const
    {
        return _layers.size();
    }

    void Map::Serialize(utilities::JsonSerializer & serializer) const
    {
        serializer.Write("layers", _layers);
    }

    void Map::Serialize(std::ostream& os) const // TODO erase
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

    void Map::DeserializeLayers(utilities::JsonSerializer & serializer, std::shared_ptr<Layer>& spLayer)
    {
        auto type = serializer.Read<std::string>("_type");
        auto version = serializer.Read<int>("_version");

        if (type == "Input")
        {
            spLayer = std::make_shared<Input>();
        }
        else if (type == "Scale")
        {
            spLayer = std::make_shared<Coordinatewise>(layers::Layer::Type::scale);
        }
        else if (type == "Shift")
        {
            spLayer = std::make_shared<Coordinatewise>(layers::Layer::Type::shift);
        }
        else if (type == "Sum")
        {
            spLayer = std::make_shared<Sum>();
        }
        else
        {
            throw std::runtime_error("unidentified type in map file: " + type);
        }

        spLayer->Deserialize(serializer, version);
    }

    std::shared_ptr<std::vector<std::vector<double>>> Map::AllocateOutputs() const
    {
        auto outputs = std::make_shared<std::vector<std::vector<double>>>();
        for (uint64 i = 0; i < _layers.size(); ++i)
        {
            outputs->emplace_back(_layers[i]->Size());
        }
        return outputs;
    }
}
