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
        return _index < _outputCoordinates.size();
    }

    void Map::OutputIterator::Next()
    {
        if (IsValid())
        {
            ++_index;
        }
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
    Map::Map() : _maxInputElement(0)
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
                    _maxInputElement = std::max(inputElement, _maxInputElement);
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
        dynamic_cast<Input*>(_layers[0].get())->SetSize(_maxInputElement + 1);
    }
}
