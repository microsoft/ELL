// LayerLayout.h

# pragma once

#include "CommandLineArguments.h"

#include "types.h"

#include "Layer.h"
using layers::Layer;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <stdexcept>
using std::runtime_error;

class LayerLayout
{
public:

    /// Ctor
    ///
    LayerLayout(uint64 numElements, double layerHorizontalMargin, double layerMaxWidth, double elementWidth, double xElementSpacing, double elementHorizontalLeftPadding, double elementHorizontalRightPadding, double dotsWidth);

    /// Returns the x-axis value of the middle of the requested element
    ///
    double GetXMid(uint64 index) const;

    /// Returns the layer width
    ///
    double GetWidth() const;

    /// Returns True if the layer is presented in abbreviated form
    ///
    bool HasHidden() const;

    /// Returns True if the specified element is hidden
    ///
    bool IsHidden(uint64 index) const;

    /// Returns the x-axis value of the middle of the dots
    ///
    double GetGapXMid() const;

private:
    vector<double> _elementXMid;
    uint64 _numHiddenElements = 0;
    double _layerWidth = 0;
    double _dotsXMid = 0;
};