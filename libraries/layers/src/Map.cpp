////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.cpp (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Map.h"
#include "Stack.h"

// stl
#include <stdexcept>
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>

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

    IndexValue Map::OutputIterator::Get() const
    {
        return IndexValue{ _index, _outputs[_index] };
    }

    Map::OutputIterator::OutputIterator(std::vector<double>&& outputs) : _outputs(std::move(outputs)), _index(0)
    {}

    //
    // Map class implementataion
    //

    Map::Map()
    {
        _stack = std::make_shared<Stack>();
    }

    Map::Map(const std::shared_ptr<Stack>& layers) : _stack(layers)
    {}

    Map::Map(const std::shared_ptr<Stack>& layers, const CoordinateList& outputCoordinates) : _stack(layers), _outputCoordinates(outputCoordinates)
    {}

    CoordinateList Map::GetOutputCoordinates() const
        {
        auto outputCoordinates = _outputCoordinates;
        if (outputCoordinates.size() == 0)
            {
            if (_stack->NumLayers() == 1)
                {
                // size should be max of what we've seen and the input layer size
                auto maxOutputSize = std::max(_maxInputSizeSeen, _stack->GetLayer(0).Size());
                if (maxOutputSize == 0)
                {
                    throw std::runtime_error("Error: unable to compute Map output coordinates");
                }

                outputCoordinates = CoordinateList(maxOutputSize);
                for (uint64 elementIndex = 0; elementIndex < maxOutputSize; ++elementIndex)
                {
                    outputCoordinates[elementIndex] = { 0, elementIndex };
                }
            }
            else
            {
                outputCoordinates = _stack->GetCoordinateList(_stack->NumLayers() - 1);
        }
    }
        return outputCoordinates;
    }

    void Map::SetOutputCoordinates(const CoordinateList& coordinates)
        {
        _outputCoordinates = coordinates;
    }

    void Map::AllocateLayerOutputs() const
    {
        auto numLayers = _stack->NumLayers();
        _layerOutputs.resize(numLayers);

        for (uint64 layerIndex = 0; layerIndex < numLayers; ++layerIndex)
        {
            auto layerSize = _stack->GetLayer(layerIndex).Size();
            if (layerIndex == 0 && numLayers == 1) // input layer
        {
                layerSize = std::max(layerSize, _maxInputSizeSeen);
    }
            _layerOutputs[layerIndex].resize(layerSize);
    }
    }

    void Map::IncreaseInputLayerSize(uint64 minSize) const
    {
        _maxInputSizeSeen = std::max(minSize, _maxInputSizeSeen);
    }
}
