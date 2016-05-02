////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapInterface.h"
#include "ModelInterface.h"

// layers
#include "Coordinate.h"
#include "CoordinateList.h"
#include "Model.h"
#include "Map.h"

// linear
#include "IndexValue.h"

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <cstdint>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>

namespace interfaces
{
    Map::Map(layers::Map&& map) : _map(std::move(map)) {}

    Map::Map(const interfaces::Model& layers, const layers::CoordinateList& outputCoordinates) : _map(std::forward<const layers::Model&>(layers.GetModel()), outputCoordinates)
    {
    }

    std::vector<double> Map::Compute(const std::vector<double>& input) const
    {
        auto it = utilities::MakeStlIndexValueIterator(input);
        auto mapOutputIter = _map.Compute(it);
        auto outputSize = _map.GetOutputCoordinateList().Size();
        std::vector<double> result(outputSize);
        while (mapOutputIter.IsValid())
        {
            auto indexValue = mapOutputIter.Get();
            result[indexValue.index] = indexValue.value;
            mapOutputIter.Next();
        }
        return result;
    }

    uint64_t Map::NumLayers() const
    {
        return _map.GetModel().NumLayers();
    }

    layers::CoordinateList Map::GetOutputCoordinateList() const
    {
        return _map.GetOutputCoordinateList();
    }

    const layers::Map& Map::GetMap() const    
     { 
         return _map; 
     }
}