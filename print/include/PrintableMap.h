// PrintableMap.h

#pragma once

#include "IPrintable.h"
#include "CommandLineArgs.h"

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
    void Print(ostream& os, const CommandLineArgs& args);

    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<IPrintable>& up);

private:
    vector<shared_ptr<IPrintable>> _printables;
};
