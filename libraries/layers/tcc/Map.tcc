////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.tcc (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "JsonSerializer.h"
#include "Files.h"

// stl
#include <algorithm>
#include <string>
#include <memory>
#include <algorithm>

namespace layers
{
    // specialization for IndexValueIterators
    template<typename IteratorType>
    void SetArray(std::vector<double>& array, IteratorType& indexValueIterator)
    {
        std::fill(array.begin(), array.end(), 0); // this isn't necessary because we always call this with a fresh array
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            if (entry.index >= array.size())
            {
                array.resize(entry.index + 1);
            }
            array[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }

    template<typename IndexValueIteratorType, typename concept>
    Map::OutputIterator Map::Compute(IndexValueIteratorType inputIterator, const CoordinateList& outputCoordinates) const
    {
        auto layerOutputs = AllocateLayerOutputs();
        
        // set the input 
        SetArray(layerOutputs[0], inputIterator);
        
        // (yuck) increment the size of the input layer if this input vector is larger than something we've seen before
        _maxInputSize = std::max(_maxInputSize, (uint64)layerOutputs[0].size());
        UpdateInputLayer();

        // compute layers 1,2,... in order
        for (uint64 i = 1; i < _layers.size(); ++i)
        {
            _layers[i]->Compute(layerOutputs, layerOutputs[i]);
        }
        
        // copy the outputs to a vector
        auto outputSize = outputCoordinates.size();
        std::vector<double> outputs(outputSize);
        for(uint64 index = 0; index < outputSize; ++index)
        {
            auto coordinate = outputCoordinates[index];
            auto layerIndex = coordinate.GetLayerIndex();
            auto elementIndex = coordinate.GetElementIndex();
            outputs[index] = layerOutputs[layerIndex][elementIndex];
        }        

        OutputIterator outputIterator(std::move(outputs));
        return outputIterator;
    }

    template <typename LayerType>
    const LayerType& Map::GetLayer(uint64 layerIndex) const
    {
        return dynamic_cast<const LayerType&>(*_layers[layerIndex]);
    }

    template<typename MapType>
    MapType Map::Load(const std::string& inputMapFile)
    {
        auto inputMapFStream = utilities::OpenIfstream(inputMapFile);
        return utilities::JsonSerializer::Load<MapType>(inputMapFStream, "Base");
    }

}
