////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     LayerLayout.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

# pragma once

#include "PrintArguments.h"

// types.h
#include "types.h"

struct Point
{
    double x;
    double y;
};

// stl
#include <vector>

class LayerLayout
{
public:

    /// Ctor
    ///
    LayerLayout(double left, double top, uint64 size, double layerMaxWidth, ElementLayoutArguments Arguments);

    /// \returns the layer width
    ///
    double GetWidth() const;

    /// \returns the layer height
    ///
    double GetHeight() const;

    /// \returns True if the layer is presented in abbreviated form
    /// 
    bool HasHidden() const;

    /// \returns The number of visible (non-hidden) elements
    ///
    uint64 NumVisibleElements() const;

    /// \returns True if the specified element is hidden
    ///
    bool IsHidden(uint64 index) const;

    /// \returns the x-axis value of the middle of the requested element
    ///
    double GetMidY() const;

    /// \returns the x-axis value of the middle of the requested element
    ///
    double GetMidX(uint64 index) const;

    /// \returns the x-axis value of the middle of the dots
    ///
    double GetDotsMidX() const;

    /// Returns the begin-point of an arrow
    ///
    Point GetOutputPoint(uint64 index) const;

    /// Returns the end-point of an arrow
    ///
    Point GetInputPoint(uint64 index) const;

private:
    double _layerWidth = 0;
    double _layerHeight = 0;
    uint64 _numHiddenElements = 0;
    std::vector<double> _elementMidX;
    double _dotsMidX = 0;
    double _midY = 0;
    double _connectorOffset = 0;
};