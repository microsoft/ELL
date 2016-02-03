// PrintableMap.h

#pragma once

#include "PrintArguments.h" 

// layers
#include "Layer.h"
#include "Map.h"

// utilities
#include "JsonSerializer.h"

// stl
#include <iostream>
#include <vector>
#include <memory>

class PrintableMap : public layers::Map
{
public:
    /// Prints a human-friendly description of the layer to a stream
    ///
    void Print(ostream& os, const PrintArguments& Arguments);

    /// Deserializes the Map in json format
    ///
    void Deserialize(utilities::JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<layers::Layer>& up);
};
