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
#include "Stack.h"

// stl
#include <stdexcept>
#include <string>

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

    uint64 Stack::AddLayer(std::unique_ptr<Layer>&& layer)
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
                    maxInputSize = std::max(inputElement + 1, maxInputSize);
                }
                inputCoords.Next();
            }
        }

        // Update input layer (layer 0)
        // #### 
        IncreaseInputLayerSize(maxInputSize);

        uint64 layerIndex = _layers.size();
        _layers.push_back(std::move(layer));
        return layerIndex;
    }

    uint64 Stack::NumLayers() const
    {
        return _layers.size();
    }

    const char* Stack::GetTypeName()
    {
        return "Map";
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

    void Stack::IncreaseInputLayerSize(uint64 minSize) const
    {
        // #### ???
        GetLayer<Input&>(0).IncreaseSize(minSize);
    }

}
