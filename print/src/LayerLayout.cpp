////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     LayerLayout.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LayerLayout.h"

LayerLayout::LayerLayout(double left, double top, uint64 size, double layerMaxWidth, ElementLayoutArguments Arguments) : _elementMidX(size)
{
    _connectorOffset = Arguments.height / 2.0;
    _layerHeight = Arguments.height + 2.0 * Arguments.verticalPadding;
    _midY = top + _layerHeight / 2.0;

    // width of the layer without abbreviated format 
    double layerWidth = Arguments.leftPadding + Arguments.rightPadding + size * Arguments.width + (size - 1) * Arguments.horizontalSpacing;

    double firstElementMidX = left + Arguments.leftPadding + Arguments.width / 2.0;
    double elementWidthPlusSpacing = (Arguments.width + Arguments.horizontalSpacing);

    // non-abbreviated
    if(layerWidth <= layerMaxWidth)
    {
        _layerWidth = layerWidth;
        _numHiddenElements = 0;

        for(uint64 k = 0; k<size; ++k)
        {
            _elementMidX[k] = firstElementMidX + k * elementWidthPlusSpacing;
        }
    }
    
    // abbreviated layer: shows as many visible elements as possible, followed by a gap, followed by one last element
    else
    {
        _layerWidth = layerMaxWidth;

        double elementsWidth = layerMaxWidth - Arguments.leftPadding - Arguments.rightPadding;
        uint64 numVisibleElements = uint64((elementsWidth - Arguments.dotsWidth) / elementWidthPlusSpacing);
        
        if(elementsWidth <= Arguments.dotsWidth || numVisibleElements < 2)
        {
            throw std::runtime_error("unable to visualize layer within the specified constraints (increase width, decrease dots width or element width/spacing)");
        }

        _numHiddenElements = size - numVisibleElements;

        // elements before gap
        for(uint64 k = 0; k<numVisibleElements-1; ++k)
        {
            _elementMidX[k] = firstElementMidX + k * elementWidthPlusSpacing;
        }

        // elements represented by dots
        double dotsLeft = left + Arguments.leftPadding + (numVisibleElements-1) * elementWidthPlusSpacing;
        double dotsRight = _layerWidth - Arguments.rightPadding - elementWidthPlusSpacing;
        _dotsMidX = (dotsLeft + dotsRight) / 2.0;
        double hiddenElementsSpacing = (dotsRight - dotsLeft) / (_numHiddenElements -1);

        for(uint64 k = numVisibleElements - 1; k<size-1; ++k)
        {
            _elementMidX[k] = dotsLeft + (k - numVisibleElements + 1) * hiddenElementsSpacing;
        }

        /// element after dots
        _elementMidX[size-1] = dotsRight + Arguments.horizontalSpacing + Arguments.width/2.0;
    }
}

double LayerLayout::GetWidth() const
{
    return _layerWidth;
}

double LayerLayout::GetHeight() const
{
    return _layerHeight;
}

bool LayerLayout::HasHidden() const
{
    if (_numHiddenElements > 0)
    {
        return true;
    }
    return false;
}

uint64 LayerLayout::NumVisibleElements() const
{
    return _elementMidX.size() - _numHiddenElements;
}

bool LayerLayout::IsHidden(uint64 index) const
{
    uint64 size = _elementMidX.size();
    if (index == size - 1 || index < size - 1 - _numHiddenElements)
    {
        return false;
    }
    return true;
}

double LayerLayout::GetMidY() const
{
    return _midY;
}

double LayerLayout::GetMidX(uint64 index) const
{
    return _elementMidX[index];
}

double LayerLayout::GetDotsMidX() const
{
    return _dotsMidX;
}

Point LayerLayout::GetOutputPoint(uint64 index) const
{
    return Point{ GetMidX(index), _midY + _connectorOffset };
}

Point LayerLayout::GetInputPoint(uint64 index) const
{
    return Point{ GetMidX(index), _midY - _connectorOffset };
}
