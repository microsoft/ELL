////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     LayerLayout.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

# pragma once

#include "PrintArguments.h"

// stl
#include <cstdint>
#include <vector>

struct Point
{
    double x;
    double y;
};

/// <summary> A layer layout. </summary>
class LayerLayout
{
public:

    /// <summary> Constructs an instance of LayerLayout. </summary>
    ///
    /// <param name="left"> The left. </param>
    /// <param name="top"> The top. </param>
    /// <param name="size"> The size. </param>
    /// <param name="layerMaxWidth"> Width of the layer maximum. </param>
    /// <param name="Arguments"> The arguments. </param>
    LayerLayout(double left, double top, uint64_t size, double layerMaxWidth, ElementLayoutArguments Arguments);

    /// <summary> Gets the width. </summary>
    ///
    /// <returns> The width. </returns>
    double GetWidth() const;

    /// <summary> Gets the height. </summary>
    ///
    /// <returns> The height. </returns>
    double GetHeight() const;

    /// <summary> Query if this object has hidden. </summary>
    ///
    /// <returns> true if hidden, false if not. </returns>
    bool HasHidden() const;

    /// <summary> Number visible elements. </summary>
    ///
    /// <returns> The total number of visible elements. </returns>
    uint64_t NumVisibleElements() const;

    /// <summary> Query if 'index' input stream hidden. </summary>
    ///
    /// <param name="index"> Zero-based index of the. </param>
    ///
    /// <returns> true if it succeeds, false if it fails. </returns>
    bool IsHidden(uint64_t index) const;

    /// <summary> Gets middle y coordinate. </summary>
    ///
    /// <returns> The middle y coordinate. </returns>
    double GetMidY() const;

    /// <summary> Gets middle x coordinate. </summary>
    ///
    /// <param name="index"> Zero-based index of the. </param>
    ///
    /// <returns> The middle x coordinate. </returns>
    double GetMidX(uint64_t index) const;

    /// <summary> Gets dots middle x coordinate. </summary>
    ///
    /// <returns> The dots middle x coordinate. </returns>
    double GetDotsMidX() const;

    /// <summary> Gets output point. </summary>
    ///
    /// <param name="index"> Zero-based index of the. </param>
    ///
    /// <returns> The output point. </returns>
    Point GetOutputPoint(uint64_t index) const;

    /// <summary> Gets input point. </summary>
    ///
    /// <param name="index"> Zero-based index of the. </param>
    ///
    /// <returns> The input point. </returns>
    Point GetInputPoint(uint64_t index) const;

private:
    double _layerWidth = 0;
    double _layerHeight = 0;
    uint64_t _numHiddenElements = 0;
    std::vector<double> _elementMidX;
    double _dotsMidX = 0;
    double _midY = 0;
    double _connectorOffset = 0;
};