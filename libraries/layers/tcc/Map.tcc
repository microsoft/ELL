////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.tcc (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CoordinateListTools.h"

// utilities
#include "XMLSerialization.h"
#include "Files.h"

// stl
#include <algorithm>
#include <string>
#include <memory>
#include <algorithm>

namespace layers
{
    // specialization for IndexValueIterators
    template<typename IndexValueIteratorType>
    void SetArray(std::vector<double>& array, IndexValueIteratorType& indexValueIterator)
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

    template<typename IndexValueIteratorType>
    void Map::LoadInputLayer(IndexValueIteratorType& inputIterator, std::vector<double>& layerOutputs) const
    {
        // set the input 
        SetArray(layerOutputs, inputIterator);
        
        // update the input layer to handle vectors of this size
        UpdateInputLayer((uint64)layerOutputs.size());
    }

    template<typename IndexValueIteratorType, typename concept>
    Map::OutputIterator Map::Compute(IndexValueIteratorType inputIterator) const
    {
        auto layerOutputs = AllocateLayerOutputs();
        
        LoadInputLayer(inputIterator, layerOutputs[0]);

        // compute layers 1,2,... in order
        for (uint64 i = 1; i < _layers.size(); ++i)
        {
            _layers[i]->Compute(layerOutputs, layerOutputs[i]);
        }
        
        if (_outputCoordinates.size() == 0)
        {
            _outputCoordinates = GetCoordinateList(*this, NumLayers() - 1);
        }

        // copy the outputs to a vector
        auto outputSize = _outputCoordinates.size();
        std::vector<double> outputs(outputSize);
        for(uint64 index = 0; index < outputSize; ++index)
        {
            auto coordinate = _outputCoordinates[index];
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
        utilities::XMLDeserializer deserializer(inputMapFStream);

        MapType map;
        deserializer.Deserialize(map);
        return map;
    }

}
