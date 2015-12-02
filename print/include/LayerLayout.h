// LayerLayout.h

# pragma once

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

    LayerLayout(double xOffset, double yOffset, double maxX, uint64 numElement);

    double GetXMidOffset(uint64 outputIndex) const;

    double GetYTopOffset(uint64 outputIndex) const;

    double GetYBottomOffset(uint64 outputIndex) const;

private:
    const double _gapToFirstElement = 60;
    const double _gapAfterLastElement = 10;
    const double _elementWidth = 50;
    const double _elementHeight = 30;
    const double _elementSpacing = 5;
    const double _dotsGap = 80;
    const double _xOffset = 0;
    const double _yOffset = 0;
    const double _maxX = 0;
    
    vector<double> _elementXMidOffset;
    double _layerRight = 0;
    bool _abbreviated = false;
    double _dotsXOffset = 0;



};