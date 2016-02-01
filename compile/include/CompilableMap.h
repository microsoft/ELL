// CompilableMap.h

#pragma once

#include "CompilableLayer.h"

// layers
#include "Map.h"
using layers::Map;
using layers::Layer;

#include "Coordinate.h"
using layers::CoordinateList;

// utilites
#include "JsonSerializer.h"
using utilities::JsonSerializer;

// stl
#include <iostream>
using std::ostream;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;


class CompilableMap : public Map
{
public:
    /// Generates C code that encodes the map
    ///
    void ToCode(CoordinateList coordinateList) const;

    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<Layer>& up);

private:

    /// define all variables and set to zero (including outputs)
    /// Set actions that correspond to output values
    /// BackwardPass()
    /// ForwardPass()
};
