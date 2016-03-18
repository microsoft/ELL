////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Coordinatewise.h"
#include "Input.h"
#include "Sum.h"
#include "Map.h"

// stl
#include <stdexcept>
#include <string>

namespace layers
{
    const int Map::_currentVersion;

    //
    // Map::OutputIterator implementation
    //
    bool Map::OutputIterator::IsValid() const
    {
        return _index < _outputCoordinates.size();
    }

    void Map::OutputIterator::Next()
    {
        ++_index;
    }

    void Map::OutputIterator::AllocateLayerOutputs(const std::vector<std::unique_ptr<Layer>>& layers)
    {
        _layerOutputs.clear();
        for (uint64 i = 0; i < layers.size(); ++i)
        {
            _layerOutputs.emplace_back(layers[i]->Size());
        }
    }

    /// \returns The current index-value pair
    ///
    IndexValue Map::OutputIterator::Get() const
    {
        auto coordinate = _outputCoordinates[_index];
        uint64 layerIndex = coordinate.GetLayerIndex();
        uint64 elementIndex = coordinate.GetElementIndex();
        return IndexValue{ _index, _layerOutputs[layerIndex][elementIndex] };
    }

    Map::OutputIterator::OutputIterator(const std::vector<std::unique_ptr<Layer>>& layers, const CoordinateList& outputCoordinates) :
        _outputCoordinates(outputCoordinates),
        _index(0)
    {
        AllocateLayerOutputs(layers);
    }


    //
    // Map class implementation
    //
    Map::Map(uint64 inputLayerSize)
    {
        _layers.push_back(std::make_unique<Input>(inputLayerSize));
    }

    uint64 Map::AddLayer(std::unique_ptr<Layer>&& layer)
    {
        uint64 layerIndex = _layers.size();
        _layers.push_back(std::move(layer));
        return layerIndex;
    }

    uint64 Map::NumLayers() const
    {
        return _layers.size();
    }

    const char* Map::GetTypeName()
    {
        return "Map";
    }

    void Map::Read(utilities::XMLDeserializer& deserializer)
    {
        deserializer.RegisterPolymorphicType<Input>();
        deserializer.RegisterPolymorphicType<Coordinatewise>();
        deserializer.RegisterPolymorphicType<Sum>();

        int version = 0;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("layers", _layers);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void Map::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("layers", _layers);
    }

    void Map::Save(std::ostream& os) const
    {
        utilities::XMLSerializer serializer(os);
        serializer.Serialize(*this);
    }
}
