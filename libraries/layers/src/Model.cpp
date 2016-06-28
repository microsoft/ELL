////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.cpp (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Model.h"

// utilities
#include "Files.h"
#include "Exception.h"

// stl
#include <stdexcept>
#include <string>
#include <algorithm>
#include <memory>
#include <iostream>
#include <utility> // for std::move
#include <ostream>
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
        
        auto layerInputDimension = layer->GetInputDimension();
        auto layerOutputDimension = layer->GetOutputDimension();

        // check that layer is compatible with model
        for (uint64_t index = 0; index < layerInputDimension; index++)
        {
            auto inputCoordIterator = layer->GetInputCoordinateIterator(index);
            while (inputCoordIterator.IsValid())
            {
                auto coord = inputCoordIterator.Get();
                if(coord.GetLayerIndex() >= numLayers)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "new layer references nonexistent layers");
                }
                else if(coord.GetLayerIndex()>0 && coord.GetElementIndex() >= GetLayer(coord.GetLayerIndex()).GetOutputDimension())
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "new layer references nonexistent elements");
                }
                inputCoordIterator.Next();
            }
        }

        _layers.push_back(std::move(layer));
        return CoordinateList(_layers.size(), layerOutputDimension);
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
    
    const Layer& Model::GetLastLayer() const
    {
        return GetLayer(NumLayers()-1);
    }

    void Model::Save(std::ostream& os) const
    {
        utilities::XMLSerializer serializer(os);
        serializer.Serialize(*this);
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
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "unsupported version: " + version);
        }
    }

    void Model::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("layers", _layers);
    }
}
