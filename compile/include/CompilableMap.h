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

class CompilableMap : public layers::Map
{
public:
    /// Generates C code that encodes the map
    ///
    void ToCode(layers::CoordinateList coordinateList, ostream& os) const;

    /// Deserializes the Map in json format
    ///
    void Deserialize(utilities::JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<layers::Layer>& up);
};
