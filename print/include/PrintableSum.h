////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintableSum.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "PrintableLayer.h"
#include "LayerLayout.h"

// layers
#include "Sum.h"

// stl
#include <iostream>

/// <summary> A struct that adds printing capabilities to a layer. </summary>
struct PrintableSum : public layers::Sum, public PrintableLayer
{
public:

    /// <summary> Gets a friendly layer name. </summary>
    ///
    /// <returns> The friendly layer name. </returns>
    virtual std::string GetFriendlyLayerName() const override;

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="left"> The left. </param>
    /// <param name="top"> The top. </param>
    /// <param name="layerIndex"> Zero-based index of the layer. </param>
    /// <param name="Arguments"> The arguments. </param>
    ///
    /// <returns> A LayerLayout. </returns>
    virtual LayerLayout Print(std::ostream& os, double left, double top, uint64 layerIndex, const PrintArguments& arguments) const override;
};
