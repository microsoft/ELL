// PrintableMap.h

#pragma once

#include "IPrintableLayer.h"
#include "CommandLineArguments.h"

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <iostream>
using std::ostream;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;


class PrintableMap
{
public:
    /// Prints a human-friendly description of the layer to a stream
    ///
    void Print(ostream& os, const CommandLineArguments& args);

    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<IPrintableLayer>& up);

private:
    vector<shared_ptr<IPrintableLayer>> _layers;
};
