////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     PrintableStack.h (print)
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

/// <summary> A printable stack. </summary>
class PrintableStack
{
public:
    /// <summary> Copy constructor that constructs an instance of PrintableStack from a Stack. </summary>
    ///
    /// <param name="other"> The Map being copied. </param>
    PrintableStack(const layers::Stack& other);

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="Arguments"> The arguments. </param>
    void Print(std::ostream& os, const PrintArguments& arguments);

private:
    std::vector<std::unique_ptr<PrintableLayer>> _printableLayers;
};
