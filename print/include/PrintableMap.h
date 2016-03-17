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
};
