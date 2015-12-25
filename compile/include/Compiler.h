// Compiler.h

#pragma once

#include "Compilable.h"

#include "JsonSerializer.h"
using utilities::JsonSerializer;

#include <iostream>
using std::ostream;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;


class Compiler
{
public:
    /// Deserializes the Map in json format
    ///
    void Deserialize(JsonSerializer& serializer);

    /// Static function for deserializing shared_ptr<Layer>
    ///
    static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<Compilable>& up);

private:
    void SetActions();

    vector<shared_ptr<Compilable>> _layers;
};
