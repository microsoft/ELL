// LayerLayout.cpp

#include "LayerLayout.h"


LayerLayout::LayerLayout(uint64 numElements, double layerMaxWidth, ElementLayoutArgs args) : _elementMidX(numElements)
{
    _connectorOffset = (args.height + args.connectorRadius) / 2.0;
    _connectorRadius = args.connectorRadius;
    _layerHeight = args.height + 2.0 * args.verticalPadding;
    _midY = _layerHeight / 2.0;

    // width of the layer without abbreviated format 
    double layerWidth = args.leftPadding + args.rightPadding + numElements * args.width + (numElements - 1) * args.horizontalSpacing;

    double firstElementMidX = args.leftPadding + args.width / 2.0;
    double elementWidthPlusSpacing = (args.width + args.horizontalSpacing);

    // non-abbreviated
    if(layerWidth <= layerMaxWidth)
    {
        _layerWidth = layerWidth;
        _numHiddenElements = 0;

        for(uint64 k = 0; k<numElements; ++k)
        {
            _elementMidX[k] = firstElementMidX + k * elementWidthPlusSpacing;
        }
    }
    
    // abbreviated layer: shows as many visible elements as possible, followed by a gap, followed by one last element
    else
    {
        _layerWidth = layerMaxWidth;

        double elementsWidth = layerMaxWidth - args.leftPadding - args.rightPadding;
        uint64 numVisibleElements = uint64((elementsWidth - args.dotsWidth) / elementWidthPlusSpacing);
        
        if(elementsWidth <= args.dotsWidth || numVisibleElements < 2)
        {
            throw runtime_error("unable to visualize layer within the specified constraints (increase width, decrease dots width or element width/spacing)");
        }

        _numHiddenElements = numElements - numVisibleElements;

        // elements before gap
        for(uint64 k = 0; k<numVisibleElements-1; ++k)
        {
            _elementMidX[k] = firstElementMidX + k * elementWidthPlusSpacing;
        }

        // elements represented by dots
        double dotsLeft = args.leftPadding + (numVisibleElements-1) * elementWidthPlusSpacing;
        double dotsRight = _layerWidth - args.rightPadding - elementWidthPlusSpacing;
        _dotsMidX = (dotsLeft + dotsRight) / 2.0;
        double hiddenElementsSpacing = (dotsRight - dotsLeft) / (_numHiddenElements -1);

        for(uint64 k = numVisibleElements - 1; k<numElements-1; ++k)
        {
            _elementMidX[k] = dotsLeft + (k - numVisibleElements + 1) * hiddenElementsSpacing;
        }

        /// element after dots
        _elementMidX[numElements-1] = dotsRight + args.horizontalSpacing + args.width/2.0;
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
    uint64 numElements = _elementMidX.size();
    if (index == numElements - 1 || index < numElements - 1 - _numHiddenElements)
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
