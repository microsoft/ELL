////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     Map.cpp (layers)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Map.h"
#include "Model.h"

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
    Map::Map(const Model& model, const CoordinateList& outputCoordinateList) : _model(model), _outputCoordinateList(outputCoordinateList)
    {
        uint64_t inputLayerSize = std::max(model.GetRequiredLayerSize(0), outputCoordinateList.GetRequiredLayerSize(0));

        // allocate reusable memory needed to compute the map
        _layerOutputs.resize(_model.NumLayers());
        _layerOutputs[0].resize(inputLayerSize);

        for (uint64_t layerIndex = 1; layerIndex < _model.NumLayers(); ++layerIndex)
        {
            auto layerSize = _model.GetLayer(layerIndex).Size();
            _layerOutputs[layerIndex].resize(layerSize);
        }
    }

    const CoordinateList& Map::GetOutputCoordinateList() const
    {
        return _outputCoordinateList;
    }

    const Model& Map::GetModel() const 
    {
        return _model;
    }
}
