// Map.tcc

#include "JsonSerializer.h"

// utilities
#include "files.h"

// stl
#include <string>
#include <memory>

namespace layers
{
    template<typename IndexValueIteratorType, typename concept>
    Map::Iterator Map::Compute(IndexValueIteratorType indexValueIterator, const CoordinateList& outputCoordinates) const
    {
        // allocate memory to store the output of the map calculation
        auto outputs = AllocateOutputs(); // Does AllocateOutputs have to return std::shared_ptr? 
        
        // set the input 
        (*outputs)[0].Set(indexValueIterator);

        // compute layers 1,2,... in order
        for(uint64 i = 1; i<_layers.size(); ++i)
        {
            _layers[i]->Compute(i, *outputs);
        }
        
        return Iterator(outputs, outputCoordinates);
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
