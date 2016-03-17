////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConstructLayer.h"
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
        return _index < _outputs.size();
    }

    void Map::OutputIterator::Next()
    {
        if (IsValid())
        {
            ++_index;
        }
    }

    /// \returns The current index-value pair
    ///
    IndexValue Map::OutputIterator::Get() const
    {
        return IndexValue{ _index, _outputs[_index]};
    }

    Map::OutputIterator::OutputIterator(std::vector<double>&& outputs) : _outputs(std::move(outputs)), _index(0)
    {}

    //
    // Map class implementation
    //
    Map::Map()
    {
        _layers.push_back(std::make_unique<Input>());
    }

    uint64 Map::AddLayer(std::unique_ptr<Layer>&& layer)
    {
        uint64 maxInputSize = 0;
        auto numLayers = _layers.size();

        // Keep track of the maximum input dimension requested and make sure new layer's inputs 
        // come from previous layers only
        auto layerSize = layer->Size();
        for (uint64 index = 0; index < layerSize; index++)
        {
            auto inputCoords = layer->GetInputCoordinates(index);
            while (inputCoords.IsValid())
            {
                auto coord = inputCoords.Get();
                auto inputLayer = coord.GetLayerIndex();
                if (inputLayer >= numLayers)
                {
                    throw std::runtime_error("Error: layer using inputs from non-previous layer");
                }
                auto inputElement = coord.GetElementIndex();
                if (inputLayer == 0) // we're referring to an element of the first, Input, layer
                {
                    maxInputSize = std::max(inputElement+1, maxInputSize);
                }
                inputCoords.Next();
            }
        }

        // Update input layer (layer 0)
        UpdateInputLayer(maxInputSize);

        uint64 layerIndex = _layers.size();
        _layers.push_back(std::move(layer));
        return layerIndex;
    }

    uint64 Map::NumLayers() const
    {
        return _layers.size();
    }

    CoordinateList Map::GetOutputCoordinates() const
    {
        return _outputCoordinates;
    }

    void Map::SetOutputCoordinates(const CoordinateList& coordinates)
    {
        _outputCoordinates = coordinates;
    }

    std::vector<std::vector<double>> Map::AllocateLayerOutputs() const
    {
        auto numLayers = _layers.size();
        std::vector<std::vector<double>> layerOutputs;
        layerOutputs.resize(numLayers);
        for (uint64 layerIndex = 0; layerIndex < numLayers; ++layerIndex)
        {
            layerOutputs[layerIndex].resize(_layers[layerIndex]->Size());
            std::fill(layerOutputs[layerIndex].begin(), layerOutputs[layerIndex].end(), 0);
        }
        return layerOutputs;
    }

    const char* Map::GetTypeName()
    {
        return "Map";
    }

    void Map::Read(utilities::XMLDeserializer& deserializer)
    {
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

    void Map::UpdateInputLayer(uint64 minSize) const
    {
        GetLayer<Input&>(0).IncreaseSize(minSize);
    }
}
