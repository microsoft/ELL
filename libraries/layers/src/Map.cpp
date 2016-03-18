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
    const int Stack::_currentVersion;

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
        return IndexValue{ _index, _outputs[_index] };
    }

    Map::OutputIterator::OutputIterator(std::vector<double>&& outputs) : _outputs(std::move(outputs)), _index(0)
    {}

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


    //
    // Map
    //

    Map::Map()
    {
        // #### TODO: should the default constructor keep a null layer stack, create an empty one, or be inaccessible?
        _layerStack = std::make_shared<Stack>();
    }

    Map::Map(const std::shared_ptr<Stack>& layers) : _layerStack(layers)
    {}

    CoordinateList Map::GetOutputCoordinates() const
    {
        auto outputCoordinates = _outputCoordinates;
        if (outputCoordinates.size() == 0)
        {
            if (_layerStack->NumLayers() == 1)
            {
                // size should be max of what we've seen and the input layer size
                auto maxOutputSize = std::max(_maxInputSize, _layerStack->GetLayer(0).Size());
                if (maxOutputSize == 0)
                {
                    throw std::runtime_error("Error: unable to compute Map output coordinates");
                }
                outputCoordinates = GetCoordinateList(0, 0, maxOutputSize-1);
            }
            else
            {
                outputCoordinates = GetCoordinateList(*_layerStack, _layerStack->NumLayers() - 1);
            }
        }
        return outputCoordinates;
    }

    void Map::SetOutputCoordinates(const CoordinateList& coordinates)
    {
        _outputCoordinates = coordinates;
    }

    std::vector<std::vector<double>> Map::AllocateLayerOutputs() const
    {
        auto numLayers = _layerStack->NumLayers();
        std::vector<std::vector<double>> layerOutputs;
        layerOutputs.resize(numLayers);

        // TODO: When we keep the outputs around instead of reallocating them for every call to compute, we'll need to ensure they're big enough
        for (uint64 layerIndex = 0; layerIndex < numLayers; ++layerIndex)
        {
            auto layerSize = _layerStack->GetLayer(layerIndex).Size();
            // #### TODO: This is gross. Can we make it simpler?
            if (layerIndex == 0) // input layer
            {
                layerSize = std::max(layerSize, _maxInputSize);
            }
            layerOutputs[layerIndex].resize(layerSize);
            std::fill(layerOutputs[layerIndex].begin(), layerOutputs[layerIndex].end(), 0);
        }
        return layerOutputs;
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

    void Map::IncreaseInputLayerSize(uint64 minSize) const
    {
        _maxInputSize = std::max(minSize, _maxInputSize);
        //        GetLayer<Input&>(0).IncreaseSize(minSize);
    }


}
