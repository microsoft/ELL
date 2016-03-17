////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.tcc (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// types
#include "StlIndexValueIterator.h"

// stl
#include <vector>
#include <algorithm>
#include <utility>

namespace layers
{
    template<typename IndexValueIteratorType>
    void Map::LoadInputLayer(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const
    {
        // set the input 
        types::CopyToArray(inputIterator, layerOutputs);
        
        // update the input layer to handle vectors of this size
        IncreaseInputLayerSize((uint64_t)layerOutputs.size());
    }

    template <typename IndexValueIteratorType, types::IsIndexValueIterator<IndexValueIteratorType> concept>
    Map::OutputIterator Map::Compute(IndexValueIteratorType inputIterator) const
    {
        AllocateLayerOutputs();
        LoadInputLayer(inputIterator, _layerOutputs[0]);

        // compute layers 1,2,... in order
        for (uint64_t i = 1; i < _stack->NumLayers(); ++i)
        {
            _stack->GetLayer(i).Compute(_layerOutputs, _layerOutputs[i]);
        }

        auto outputCoordinates = GetOutputCoordinates();

        // copy the outputs to a vector
        auto outputSize = outputCoordinates.size();
        std::vector<double> outputs(outputSize);
        for(uint64_t index = 0; index < outputSize; ++index)
        {
            auto coordinate = outputCoordinates[index];
            auto layerIndex = coordinate.GetLayerIndex();
            auto elementIndex = coordinate.GetElementIndex();
            outputs[index] = _layerOutputs[layerIndex][elementIndex];
        }        

        OutputIterator outputIterator(std::move(outputs));
        return outputIterator;
    }
}
