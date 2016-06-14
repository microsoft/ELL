////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LayerLayout.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LayerLayout.h"

// utilities
#include "Exception.h"

LayerLayout::LayerLayout(double left, double top, uint64_t size, double layerMaxWidth, ElementLayoutArguments Arguments) : _elementMidX(size)
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

        for(uint64_t k = 0; k<size; ++k)
        {
            _elementMidX[k] = firstElementMidX + k * elementWidthPlusSpacing;
        }
    }
    
    // abbreviated layer: shows as many visible elements as possible, followed by a gap, followed by one last element
    else
    {
        _layerWidth = layerMaxWidth;

        double elementsWidth = layerMaxWidth - Arguments.leftPadding - Arguments.rightPadding;
        uint64_t numVisibleElements = uint64_t((elementsWidth - Arguments.dotsWidth) / elementWidthPlusSpacing);
        
        if(elementsWidth <= Arguments.dotsWidth || numVisibleElements < 2)
        {
            throw utilities::Exception(utilities::ExceptionErrorCodes::invalidArgument, "unable to visualize layer within the specified constraints (increase width, decrease dots width or element width/spacing)");
        }

        _numHiddenElements = size - numVisibleElements;

        // elements before gap
        for(uint64_t k = 0; k<numVisibleElements-1; ++k)
        {
            _elementMidX[k] = firstElementMidX + k * elementWidthPlusSpacing;
        }

        // elements represented by dots
        double dotsLeft = left + Arguments.leftPadding + (numVisibleElements-1) * elementWidthPlusSpacing;
        double dotsRight = _layerWidth - Arguments.rightPadding - elementWidthPlusSpacing;
        _dotsMidX = (dotsLeft + dotsRight) / 2.0;
        double hiddenElementsSpacing = (dotsRight - dotsLeft) / (_numHiddenElements -1);

        for(uint64_t k = numVisibleElements - 1; k<size-1; ++k)
        {
            _elementMidX[k] = dotsLeft + (k - numVisibleElements + 1) * hiddenElementsSpacing;
        }

        /// element after dots
        _elementMidX[size-1] = dotsRight + Arguments.horizontalSpacing + Arguments.width/2.0;
    }
}

bool LayerLayout::IsHidden(uint64_t index) const
{
    uint64_t size = _elementMidX.size();
    if (index == size - 1 || index < size - 1 - _numHiddenElements)
    {
        return false;
    }
    return true;
}

double LayerLayout::GetMidX(uint64_t index) const
{
    return _elementMidX[index];
}

Point LayerLayout::GetOutputPoint(uint64_t index) const
{
    return Point{ GetMidX(index), _midY + _connectorOffset };
}

Point LayerLayout::GetInputPoint(uint64_t index) const
{
    return Point{ GetMidX(index), _midY - _connectorOffset };
}
