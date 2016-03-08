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

namespace layers
{
    template<typename IndexValueIteratorType>
    void SetArray(std::vector<double>& array, IndexValueIteratorType indexValueIterator)
    {
        std::fill(array.begin(), array.end(), 0);
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            if (entry.index >= array.size())
            {
                break;
            }
            array[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }

    template<typename IndexValueIteratorType, typename concept>
    Map::Iterator Map::Compute(IndexValueIteratorType indexValueIterator, const CoordinateList& outputCoordinates) const
    {
        // allocate memory to store the output of the map calculation
        auto outputs = AllocateOutputs();
        
        // set the input 
        SetArray( outputs[0], indexValueIterator);

        // compute layers 1,2,... in order
        for(uint64 i = 1; i<_layers.size(); ++i)
        {
            _layers[i]->Compute(i, outputs);
        }
        
        return Iterator(std::move(outputs), outputCoordinates);
    }

    template<typename LayerType>
    std::shared_ptr<const LayerType> Map::GetLayer(uint64 layerIndex) const
    {
        return std::dynamic_pointer_cast<LayerType>(_layers[layerIndex]);
    }

    template<typename MapType>
    MapType Map::Load(const std::string& inputMapFile)
    {
        auto inputMapFStream = utilities::OpenIfstream(inputMapFile);
        return utilities::JsonSerializer::Load<MapType>(inputMapFStream, "Base");
    }

}
