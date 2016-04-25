////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Stack.cpp (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Stack.h"

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
    const int Stack::_currentVersion;

    //
    // Stack class implementation
    //
    uint64_t Stack::AddLayer(std::unique_ptr<Layer> layer)
    {
        // check that the layer points to valid elements
        auto numLayers = NumLayers();
        auto layerSize = layer->Size();
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
        return _layers.size();
    }

    uint64_t Stack::NumLayers() const
    {
        // add one, to account for the input layer, which is not explicitly stores in _layers
        return _layers.size()+1;
    }

    uint64_t Stack::GetRequiredLayerSize(uint64_t layerIndex) const
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

    const Layer& Stack::GetLayer(uint64_t layerIndex) const
    {
        assert(layerIndex > 0);

        // recall that _layers does not explicitly keep the input layer
        return *_layers[layerIndex - 1];
    }

    CoordinateList Stack::BuildCoordinateList(uint64_t layerIndex) const
    {
        if (layerIndex == 0)
        {
            throw std::runtime_error("input layer does not have an input coordinate list");
        }

        return CoordinateList(layerIndex, GetLayer(layerIndex).Size());
    }

    //Stack Stack::Load(const std::string& inputStackFile)
    //{
    //    auto inputMapFStream = utilities::OpenIfstream(inputStackFile);
    //    utilities::XMLDeserializer deserializer(inputMapFStream);

    //    Stack stack;
    //    deserializer.Deserialize(stack);
    //    return stack;
    //}

    void Stack::Save(std::ostream& os) const
    {
        utilities::XMLSerializer serializer(os);
        serializer.Serialize(*this);
    }

    std::string Stack::GetTypeName()
    {
        return "Stack";
    }

    void Stack::Read(utilities::XMLDeserializer& deserializer)
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

    void Stack::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("layers", _layers);
    }
}
