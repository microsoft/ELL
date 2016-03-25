////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Stack.cpp (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Input.h"
#include "Stack.h"

// stl
#include <stdexcept>
#include <string>
#include <algorithm>
#include <memory>
#include <iostream>
#include <utility> // for std::move

namespace layers
{
    const int Stack::_currentVersion;

    //
    // Stack class implementation
    //
    Stack::Stack()
    {
        _layers.push_back(std::make_unique<Input>());
    }

    uint64_t Stack::AddLayer(std::unique_ptr<Layer>&& layer)
    {
        uint64_t maxInputSize = 0;
        auto numLayers = _layers.size();

        // Keep track of the maximum input dimension requested and make sure new layer's inputs 
        // come from previous layers only
        auto layerSize = layer->Size();
        for (uint64_t index = 0; index < layerSize; index++)
        {
            auto inputCoordIterator = layer->GetInputCoordinates(index);
            while (inputCoordIterator.IsValid())
            {
                auto coord = inputCoordIterator.Get();
                auto inputLayer = coord.GetLayerIndex();
                if (inputLayer >= numLayers)
                {
                    throw std::runtime_error("Error: layer using inputs from non-previous layer");
                }
                auto inputElement = coord.GetElementIndex();
                if (inputLayer == 0) // we're referring to an element of the first, Input, layer
                {
                    maxInputSize = std::max(inputElement + 1, maxInputSize);
                }
                inputCoordIterator.Next();
            }
        }

        // Update input layer to be at least as big as the largest element this new layer accesses
        IncreaseInputLayerSize(maxInputSize);

        uint64_t layerIndex = _layers.size();
        _layers.push_back(std::move(layer));
        return layerIndex;
    }

    uint64_t Stack::NumLayers() const
    {
        return _layers.size();
    }

    CoordinateList Stack::GetCoordinateList(uint64_t layerIndex) const
    {
        auto layerSize = _layers[layerIndex]->Size();
        CoordinateList coordinateList(layerSize);
        for (uint64_t elementIndex = 0; elementIndex < layerSize; ++elementIndex)
        {
            coordinateList[elementIndex] = { layerIndex, elementIndex };
        }
        return coordinateList;
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

    void Stack::Save(std::ostream& os) const
    {
        utilities::XMLSerializer serializer(os);
        serializer.Serialize(*this);
    }

    void Stack::IncreaseInputLayerSize(uint64_t minSize)
    {
        GetLayer<Input&>(0).IncreaseSize(minSize);
    }

}
