////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.tcc (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <vector>
#include <algorithm>

namespace layers
{
    template<typename IndexValueIteratorType>
    void Map::InitializeLayerOutputs(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const
    {
        // set all outputs to zero 
        for(auto& outputs : _layerOutputs)
        {
            std::fill(outputs.begin(), outputs.end(), 0);
        }

        // set the input layer
        auto inputLayerSize = _layerOutputs[0].size();
        while(inputIterator.IsValid())
        {
            auto entry = inputIterator.Get();
            if(entry.index >= inputLayerSize)
            {
                return;
            }
            _layerOutputs[0][entry.index] = entry.value;
            inputIterator.Next();
        }
    }

    template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept>
    Map::OutputIterator Map::Compute(IndexValueIteratorType inputIterator) const
    {
        InitializeLayerOutputs(inputIterator, _layerOutputs[0]);

        // compute layers 1,2,... in order
        for (uint64_t i = 1; i < _model.NumLayers(); ++i)
        {
            _model.GetLayer(i).Compute(_layerOutputs, _layerOutputs[i]);
        }

        // copy the outputs to a vector
        auto outputSize = _outputCoordinateList.Size();
        std::vector<double> outputs(outputSize);
        for(uint64_t index = 0; index < outputSize; ++index)
        {
            auto coordinate = _outputCoordinateList[index];
            auto layerIndex = coordinate.GetLayerIndex();
            auto elementIndex = coordinate.GetElementIndex();
            outputs[index] = _layerOutputs[layerIndex][elementIndex];
        }        

        OutputIterator outputIterator(std::move(outputs));
        return outputIterator;
    }
}
