// LayerLayout.h

# pragma once

#include "PrintArguments.h"

#include "types.h"

#include "Layer.h"
using layers::Layer;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <stdexcept>
using std::runtime_error;

struct Point
{
    double x;
    double y;
};

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
    vector<double> _elementMidX;
    double _dotsMidX = 0;
    double _midY = 0;
    double _connectorOffset = 0;
};