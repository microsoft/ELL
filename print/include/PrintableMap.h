////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintableMap.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "PrintArguments.h" 

// layers
#include "Layer.h"
#include "Map.h"

// utilities
#include "JsonSerializer.h"
#include "XMLSerialization.h"

// stl
#include <iostream>
#include <memory>

/// <summary> A printable map. </summary>
class PrintableMap : public layers::Map
{
public:

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="Arguments"> The arguments. </param>
    void Print(std::ostream& os, const PrintArguments& Arguments);

    // TODO remove

    /// <summary> Deserializes the Map in json format. </summary>
    ///
    /// <param name="serializer"> [in,out] The serializer. </param>
    void Deserialize(utilities::JsonSerializer& serializer);

    /// <summary> Static function for deserializing shared_ptr<Layer> </summary>
    ///
    /// <param name="serializer"> [in,out] The serializer. </param>
    /// <param name="up"> [in,out] The up. </param>
    static void DeserializeLayers(utilities::JsonSerializer& serializer, std::unique_ptr<layers::Layer>& up);
};
