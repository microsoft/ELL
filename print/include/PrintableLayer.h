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

// layers
#include "Layer.h"
#include "Coordinate.h"

// utilities
#include "StlIterator.h"

#include <iostream>
#include <memory>

/// <summary> An interface for classes that can print human-friendly descriptions of themselves. </summary>
class PrintableLayer 
{
public:
    /// <summary> default virtual destructor. </summary>
    virtual ~PrintableLayer() = default;

    /// <summary> Gets a friendly layer name. </summary>
    ///
    /// <returns> The friendly layer name. </returns>
    virtual std::string GetFriendlyLayerName() const = 0;

    /// <summary> Returns the number of elements in the layer. </summary>
    ///
    /// <returns> The number of elements in the layer. </returns>
    virtual uint64 Size() const = 0;

    /// <summary> Prints a SVG description of the underlying class to an output stream. </summary>
    ///
    /// <param name="os"> [in,out] Stream to write data to. </param>
    /// <param name="left"> The left. </param>
    /// <param name="top"> The top. </param>
    /// <param name="layerIndex"> Zero-based index of the layer. </param>
    /// <param name="Arguments"> The arguments. </param>
    ///
    /// <returns> A LayerLayout. </returns>
    virtual LayerLayout Print(std::ostream& os, double left, double top, uint64 layerIndex, const PrintArguments& arguments) const = 0;

    /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
    ///
    /// <param name="index"> Zero-based element index. </param>
    ///
    /// <returns> The input coordinates. </returns>
    virtual layers::Layer::InputCoordinateIterator GetInputCoordinates(uint64 index) const = 0;

    /// <summary> Assignment operator from layer to PrintableLayer. </summary>
    ///
    /// <param name="layer"> The layer being copied. </param>
    virtual void operator=(const layers::Layer& layer) = 0;

    /// <summary> Gets the name of this type. </summary>
    ///
    /// <returns> The name of this type. </returns>
    static std::string GetTypeName(); 

protected:
    LayerLayout Print(std::ostream& os, double left, double top, uint64 layerIndex, const std::string& typeName, uint64 size, const ElementLayoutArguments& elementLayout, const LayerStyleArguments& layerStyle) const;
};

 