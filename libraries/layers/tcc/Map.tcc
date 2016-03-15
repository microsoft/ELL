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
#include "XMLSerialization.h"
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
        std::fill(array.begin(), array.end(), 0); // this isn't necessary because we always call this with a fresh array
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            if (entry.index >= array.size()) // assume indexValueIterator is sorted by index
            {
                break;
            }
            array[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }

    template<typename IndexValueIteratorType, typename concept>
    Map::OutputIterator Map::Compute(IndexValueIteratorType inputIterator, const CoordinateList& outputCoordinates) const
    {
        OutputIterator outputIterator(_layers, outputCoordinates);

        // set the input 
        SetArray(outputIterator._layerOutputs[0], inputIterator);

        // compute layers 1,2,... in order
        for (uint64 i = 1; i < _layers.size(); ++i)
        {
            _layers[i]->Compute(i, outputIterator._layerOutputs);
        }

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
        utilities::XMLDeserializer deserializer(inputMapFStream);

        MapType map;
        deserializer.Deserialize("TODO", map);

        return map;
    }

}
