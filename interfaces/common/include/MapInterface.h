////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapInterface.h (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelInterface.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateList.h"
#include "Model.h"

// linear
#include "IndexValue.h"

// stl
#include <memory>
#include <vector>

namespace interfaces
{
/// <summary> Implements a map. </summary>
class Map
{
public:
    Map(layers::Map&& map);
    
    /// <summary> Constructs an instance of pointing to an existing model, using the given output coordinates. </summary>
    Map(const interfaces::Model& layers, const layers::CoordinateList& outputCoordinates);

    /// <summary> Virtual destructor. </summary>
    virtual ~Map() = default;

    /// <summary> Computes the Map. </summary>
    ///
    /// <typeparam name="IndexValueIteratorType"> Input iterator type. </typeparam>
    /// <param name="IndexValueIterator"> The input value iterator. </param>
    ///
    /// <returns> An Iterator over output values. </returns>
    std::vector<double> Compute(const std::vector<double>& input) const;

    uint64_t NumLayers() const;

    /// <summary> Returns the current output coordinates for the map. </summary>
    ///
    /// <returns> The current output coordinates. </returns>
    layers::CoordinateList GetOutputCoordinateList() const;

    /// <summary> Returns the layers::Map object wrapped by this wrapper </summary>
    ///
    /// <returns> The Map wrappted by this object </summary>
    const layers::Map& GetMap() const;
    
private:    
    // members
    layers::Map _map;
};
}