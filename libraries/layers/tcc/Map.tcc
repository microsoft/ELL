// Map.tcc

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <string>
using std::string;
using std::istreambuf_iterator;

#include <memory>
using std::make_shared;
using std::dynamic_pointer_cast;

namespace layers
{
    template<typename IndexValueIteratorType, typename concept>
    Map::Iterator Map::Compute(IndexValueIteratorType indexValueIterator, const CoordinateList& outputCoordinates) const
    {
        // allocate memory to store the output of the map calculation
        auto outputs = AllocateOutputs(); // Does AllocateOutputs have to return shared_ptr? 
        
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
    shared_ptr<const LayerType> Map::GetLayer(uint64 layerIndex) const
    {
        return dynamic_pointer_cast<LayerType>(_layers[layerIndex]);
    }
}