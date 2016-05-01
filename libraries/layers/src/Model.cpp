////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Model.cpp (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Model.h"

// utilities
#include "Files.h"

// stl
#include <stdexcept>
#include <string>
#include <algorithm>
#include <memory>
#include <iostream>
#include <cassert>

namespace layers
{
    const int Model::_currentVersion;

    //
    // Model class implementation
    //
    CoordinateList Model::AddLayer(std::unique_ptr<Layer> layer)
    {
        // check that the layer points to valid elements
        auto numLayers = NumLayers();
        
        auto layerSize = layer->Size();

        // check that layer is compatible with model
        for (uint64_t index = 0; index < layerSize; index++)
        {
            auto inputCoordIterator = layer->GetInputCoordinateIterator(index);
            while (inputCoordIterator.IsValid())
            {
                auto coord = inputCoordIterator.Get();
                if(coord.GetLayerIndex() >= numLayers)
                {
                    throw std::runtime_error("new layer references nonexistent layers");
                }
                else if(coord.GetLayerIndex()>0 && coord.GetElementIndex() >= GetLayer(coord.GetLayerIndex()).Size())
                {
                    throw std::runtime_error("new layer references nonexistent elements");
                }
                inputCoordIterator.Next();
            }
        }

        _layers.push_back(std::move(layer));
        return CoordinateList(_layers.size(), layerSize);
    }

    uint64_t Model::NumLayers() const
    {
        // add one, to account for the input layer, which is not explicitly stores in _layers
        return _layers.size()+1;
    }

    uint64_t Model::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        uint64_t max = 0;
        for (const auto& layer : _layers)
        {
            auto requiredSize = layer->GetRequiredLayerSize(layerIndex);
            if (requiredSize > max)
            {
                max = requiredSize;
            }
        }
        return max;
    }

    const Layer& Model::GetLayer(uint64_t layerIndex) const
    {
        assert(layerIndex > 0);

        // recall that _layers does not explicitly keep the input layer
        return *_layers[layerIndex - 1];
    }

    void Model::Save(std::ostream& os) const
    {
        utilities::XMLSerializer serializer(os);
        serializer.Serialize(*this);
    }

    std::string Model::GetTypeName()
    {
        return "Model";
    }

    void Model::Read(utilities::XMLDeserializer& deserializer)
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

    void Model::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("layers", _layers);
    }
}
