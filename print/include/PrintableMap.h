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
#include "PrintableLayer.h"
#include "PrintableSum.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"

// layers
#include "Layer.h"
#include "Map.h"

// utilities
#include "TypeFactory.h"

// stl
#include <iostream>
#include <memory>

/// <summary> A printable Map. </summary>
class PrintableMap : public layers::Map
{
public:

    /// <summary> Copy constructor that constructs an instance of PrintableMap from a Map. </summary>
    ///
    /// <param name="other"> The Map being copied. </param>
    PrintableMap(const layers::Map& other);

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="Arguments"> The arguments. </param>
    void Print(std::ostream& os, const PrintArguments& arguments);

private:
    std::vector<std::unique_ptr<PrintableLayer>> _layers;
};
