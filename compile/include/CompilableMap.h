// CompilableMap.h

#pragma once

#include "CompilableLayer.h"

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <iostream>
using std::ostream;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;


class CompilableMap
{
public:

    /// Prints the code





    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<CompilableLayer>& up);

private:

    /// define all variables and set to zero (including outputs)
    /// Set actions that correspond to output values
    /// BackwardPass()
    /// ForwardPass()

    vector<shared_ptr<CompilableLayer>> _layers;
};
