////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableModel.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "PrintArguments.h" 
#include "PrintableLayer.h"
#include "PrintableSum.h"
#include "PrintableCoordinatewise.h"

// layers
#include "Layer.h"
#include "Map.h"

// utilities
#include "TypeFactory.h"

// stl
#include <iostream>
#include <memory>

/// <summary> A printable model. </summary>
class PrintableModel
{
public:
    /// <summary> Copy constructor that constructs an instance of PrintableModel from a Model. </summary>
    ///
    /// <param name="other"> The Map being copied. </param>
    PrintableModel(const layers::Model& other);

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="Arguments"> The arguments. </param>
    void Print(std::ostream& os, const PrintArguments& arguments);

private:
    uint64_t _inputLayerSize = 0;
    std::vector<std::unique_ptr<PrintableLayer>> _printableLayers;
};
