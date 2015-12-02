// LayerVisualizer.cpp

#include "LayerVisualizer.h"

LayerVisualizer::LayerVisualizer(double xOffset, double yOffset, double maxX, uint64 numElements) : _xOffset(xOffset), _yOffset(yOffset), _maxX(maxX), _elementXOffset(numElements)
{
    
    // width of the layer without abbreviated format 
    double layerWidth = _gapAfterLastElement + _gapAfterLastElement + numElements * _elementWidth + (numElements-1) * _elementSpacing;

    // non-abbreviated
    if(xOffset + layerWidth <= maxX)
    {
        double firstElementXOffset = xOffset + _gapToFirstElement + _elementWidth/2.0;
        _layerRight = xOffset + layerWidth;
        for(uint64 k = 0; k<numElements; ++k)
        {
            _elementXMidOffset[k] = firstElementXOffset + k * (_elementWidth + _elementSpacing);
        }
    }
    
    //abbreviated
    else
    {
        _layerRight = maxX;
        _abbreviated = true;

        double elementsWidth = maxX - xOffset - _gapAfterLastElement - _gapToFirstElement;
        uint64 numVisibleElements = uint64((elementsWidth - _dotsGap) / (_elementWidth + _elementSpacing));
        double firstElementXOffset = xOffset + _gapToFirstElement + _elementWidth/2.0;

        if(elementsWidth < _dotsGap || numVisibleElements < 2)
        {
            throw runtime_error("unable to visualize layer within the allocated width");
        }

        // elements before dots
        for(uint64 k = 0; k<numVisibleElements-1; ++k)
        {
            _elementXMidOffset[k] = firstElementXOffset + k * (_elementWidth + _elementSpacing);
        }

        // elements represented by dots
        double dotsLeft = firstElementXOffset + (numVisibleElements-1) * (_elementWidth + _elementSpacing) + _elementWidth/2.0;
        double dotsRight = _layerRight - _gapAfterLastElement - _elementWidth - _elementSpacing;
        _dotsXOffset = (dotsLeft + dotsRight) / 2.0;

        for(uint64 k = numVisibleElements-1; k<numElements-1; ++k)
        {
            _elementXMidOffset[k] = _dotsXOffset;
        }

        /// element after dots
        _elementXMidOffset[numElements-1] = _layerRight - _gapAfterLastElement - _elementWidth/2.0;
    }
}

double LayerVisualizer::GetXMidOffset(uint64 outputIndex) const
{
    return _elementXMidOffset[outputIndex];
}

double LayerVisualizer::GetYTopOffset(uint64 outputIndex) const
{
    return _yOffset + _elementSpacing;
}

double LayerVisualizer::GetYBottomOffset(uint64 outputIndex) const
{
    return _yOffset + _elementSpacing + _elementHeight;
}


