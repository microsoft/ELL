// CompilableMap.h

#pragma once

#include "CompilableLayer.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// utilites
#include "JsonSerializer.h"

// stl
#include <memory>

class CompilableMap : public layers::Map
{
public:
    /// Generates C code that encodes the map
    ///
    void ToCode(layers::CoordinateList coordinateList) const;

    /// Deserializes the Map in json format
    ///
    void Deserialize(utilities::JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<layers::Layer>& up);

private:

    /// define all variables and set to zero (including outputs)
    /// Set actions that correspond to output values
    /// BackwardPass()
    /// ForwardPass()
};
