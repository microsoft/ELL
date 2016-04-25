////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
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

    linear::IndexValue Map::OutputIterator::Get() const
    {
        return{ _index, _outputs[_index] };
    }

    Map::OutputIterator::OutputIterator(std::vector<double> outputs) : _outputs(std::move(outputs)), _index(0)
    {}

    //
    // Map class implementataion
    //
    Map::Map(const Stack& stack, const CoordinateList& outputCoordinateList) : _stack(stack), _outputCoordinateList(outputCoordinateList)
    {
        uint64_t inputLayerSize = std::max(stack.GetRequiredLayerSize(0), outputCoordinateList.GetRequiredLayerSize(0));

        // allocate reusable memory needed to compute the map
        _layerOutputs.resize(_stack.NumLayers());
        _layerOutputs[0].resize(inputLayerSize);

        for (uint64_t layerIndex = 1; layerIndex < _stack.NumLayers(); ++layerIndex)
        {
            auto layerSize = _stack.GetLayer(layerIndex).Size();
            _layerOutputs[layerIndex].resize(layerSize);
        }
    }

    const CoordinateList& Map::GetOutputCoordinateList() const
    {
        return _outputCoordinateList;
    }

    const Stack & Map::LoadStack() const
    {
        return _stack;
    }
}
