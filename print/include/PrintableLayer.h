////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintableLayer.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LayerLayout.h"
#include "PrintArguments.h"

#include "types.h"

#include <iostream>
#include <memory>

/// <summary> An interface for classes that can print human-friendly descriptions of themselves. </summary>
class PrintableLayer 
{
public:
    /// <summary> default virtual destructor. </summary>
    virtual ~PrintableLayer() = default;

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="left"> The left. </param>
    /// <param name="top"> The top. </param>
    /// <param name="layerIndex"> Zero-based index of the layer. </param>
    /// <param name="Arguments"> The arguments. </param>
    ///
    /// <returns> A LayerLayout. </returns>
    virtual LayerLayout Print(std::ostream& os, double left, double top, uint64 layerIndex, const PrintArguments& Arguments) const = 0;

protected:
    LayerLayout Print(std::ostream& os, double left, double top, uint64 layerIndex, const std::string& typeName, uint64 size, const ElementLayoutArguments& elementLayout, const LayerStyleArguments& layerStyle) const;
};

 