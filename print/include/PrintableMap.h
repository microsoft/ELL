// PrintableMap.h

#pragma once

#include "PrintArguments.h" 

// layers
#include "Layer.h"
using layers::Layer;

#include "Map.h"
using layers::Map;

// utilities
#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <iostream>
using std::ostream;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;


class PrintableMap : public Map
{
public:
    /// Prints a human-friendly description of the layer to a stream
    ///
    void Print(ostream& os, const PrintArguments& Arguments);

    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<Layer>& up);
};
