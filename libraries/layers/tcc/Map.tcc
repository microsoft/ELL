////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Map.tcc (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <vector>
#include <algorithm>
#include <utility>

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
        IncreaseInputLayerSize((uint64)layerOutputs.size());
    }

    template<typename IndexValueIteratorType, typename concept>
    Map::OutputIterator Map::Compute(IndexValueIteratorType inputIterator) const
    {
        AllocateLayerOutputs();
        LoadInputLayer(inputIterator, _layerOutputs[0]);

        // compute layers 1,2,... in order
        for (uint64 i = 1; i < _stack->NumLayers(); ++i)
        {
            _stack->GetLayer(i).Compute(_layerOutputs, _layerOutputs[i]);
        }

        auto outputCoordinates = GetOutputCoordinates();

        // copy the outputs to a vector
        auto outputSize = outputCoordinates.size();
        std::vector<double> outputs(outputSize);
        for(uint64 index = 0; index < outputSize; ++index)
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
