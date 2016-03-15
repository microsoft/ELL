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
    Map::Map() : _maxInputSize(0)
    {
        _layers.push_back(std::make_unique<Input>());
    }

    uint64 Map::AddLayer(std::unique_ptr<Layer>&& layer)
    {
        // #### TODO: verify layer's input coordinates make sense
        
        // Keep track of the maximum input dimension requested
        auto layerSize = layer->Size();
        for (uint64 index = 0; index < layerSize; index++)
        {
            auto inputCoords = layer->GetInputCoordinates(index);
            while (inputCoords.IsValid())
            {
                auto coord = inputCoords.Get();
                auto inputLayer = coord.GetLayerIndex();
                auto inputElement = coord.GetElementIndex();
                if (inputLayer == 0)
                {
                    _maxInputSize = std::max(inputElement+1, _maxInputSize);
                }
                inputCoords.Next();
            }
        }

        // Update input layer (layer 0)
        UpdateInputLayer();

        uint64 layerIndex = _layers.size();
        _layers.push_back(std::move(layer));
        return layerIndex;
    }

    uint64 Map::NumLayers() const
    {
        return _layers.size();
    }

    std::vector<std::vector<double>> Map::AllocateLayerOutputs() const
    {
        auto numLayers = _layers.size();
        std::vector<std::vector<double>> layerOutputs;
        layerOutputs.resize(numLayers);
        for (uint64 layerIndex = 0; layerIndex < numLayers; ++layerIndex)
        {
            layerOutputs[layerIndex].resize(_layers[layerIndex]->Size());
            layerOutputs[layerIndex].clear();
        }
        return layerOutputs;
    }

    const char* Map::GetTypeName()
    {
        return "Map";
    }

    void Map::Read(utilities::XMLDeserializer& deserializer)
    {
        //int version = 0;
        //deserializer.Deserialize("version", version);
        //if (version == 1)
        //{
        //    deserializer.Deserialize("layers", _layers);
        //}
        //else
        //{
        //    throw std::runtime_error("unsupported version: " + std::to_string(version));
        //}
    }

    void Map::Write(utilities::XMLSerializer& serializer) const
    {
        //serializer.Serialize("version", _currentVersion);
        //serializer.Serialize("layers", _layers);
    }

    void Map::Serialize(utilities::JsonSerializer& serializer) const
    {
        //serializer.Write("layers", _layers);
    }

    void Map::Serialize(std::ostream& os) const // TODO erase
    {
        //utilities::JsonSerializer writer;
        //writer.Write("Base", *this);
        //auto str = writer.ToString();
        //os << str;
    }

    void Map::Deserialize(utilities::JsonSerializer & serializer)
    {
        //serializer.Read("layers", _layers, DeserializeLayers);
    }

    void Map::DeserializeLayers(utilities::JsonSerializer & serializer, std::unique_ptr<Layer>& spLayer)
    {
        //auto type = serializer.Read<std::string>("_type");
        //auto version = serializer.Read<int>("_version");

        //if (type == "Input")
        //{
        //    spLayer = std::make_shared<Input>();
        //}
        //else if (type == "Scale")
        //{
        //    spLayer = std::make_shared<Coordinatewise>(layers::Layer::Type::scale);
        //}
        //else if (type == "Shift")
        //{
        //    spLayer = std::make_shared<Coordinatewise>(layers::Layer::Type::shift);
        //}
        //else if (type == "Sum")
        //{
        //    spLayer = std::make_shared<Sum>();
        //}
        //else
        //{
        //    throw std::runtime_error("unidentified type in map file: " + type);
        //}

        //spLayer->Deserialize(serializer, version);
    }

    void Map::UpdateInputLayer() const
    {
        dynamic_cast<Input*>(_layers[0].get())->SetSize(_maxInputSize);
    }
}
