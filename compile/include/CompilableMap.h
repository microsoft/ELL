////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableMap.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// utilites
#include "JsonSerializer.h"

// stl
#include <memory>
#include <iostream>

/// <summary> A compilable map. </summary>
class CompilableMap : public layers::Map
{
public:

    /// <summary> Generates C code that encodes the map. </summary>
    ///
    /// <param name="coordinateList"> List of output coordinates, whose values must be computed. </param>
    /// <param name="os"> [in,out] Stream to write data to. </param>
    void ToCode(layers::CoordinateList coordinateList, std::ostream& os) const;

    /// <summary> Deserializes the Map in json format. </summary>
    ///
    /// <param name="serializer"> [in,out] The serializer. </param>
    void Deserialize(utilities::JsonSerializer& serializer);

    /// <summary> Static function for deserializing shared_ptr<Layer> </summary>
    ///
    /// <param name="serializer"> [in,out] The serializer. </param>
    /// <param name="up"> [in,out] A pointer to the layer being deserialized. </param>
    static void DeserializeLayers(utilities::JsonSerializer& serializer, std::unique_ptr<layers::Layer>& upLayer);
};
