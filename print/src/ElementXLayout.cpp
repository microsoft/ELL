// ElementXLayout.cpp

#include "ElementXLayout.h"

ElementXLayout::ElementXLayout(uint64 numElements, double xLayerIndent, double maxLayerWidth, double elementWidth, double xElementSpacing, double xElementLeftPadding, double xElementRightPadding, double dotsWidth) : _elementXMid(numElements)
{
    
    // width of the layer without abbreviated format 
    double layerWidth = xElementLeftPadding + xElementRightPadding + numElements * elementWidth + (numElements-1) * xElementSpacing;
    double elementWidthPlusSpace = elementWidth + xElementSpacing;

    // non-abbreviated
    if(layerWidth <= maxLayerWidth)
    {
        _layerWidth = layerWidth;
        _numElementsBeforeDots = numElements;
        double firstElementXMiddle = xLayerIndent + xElementLeftPadding + elementWidth/2.0;
        for(uint64 k = 0; k<numElements; ++k)
        {
            _elementXMid[k] = firstElementXMiddle + k * elementWidthPlusSpace;
        }
    }
    
    //abbreviated
    else
    {
        _layerWidth = maxLayerWidth;
        _abbreviated = true;

        double elementsWidth = maxLayerWidth - xElementLeftPadding - xElementRightPadding;
        double elementsBeforeDotsWidth = elementsWidth - dotsWidth - elementWidth - xElementSpacing;
        _numElementsBeforeDots = uint64(elementsBeforeDotsWidth / (elementWidth + xElementSpacing));
        
        if(elementsBeforeDotsWidth <= 0 || _numElementsBeforeDots == 0)
        {
            throw runtime_error("unable to visualize layer within the specified constraints (increase width, decrease dots width or element width/spacing)");
        }

        // elements before dots
        double firstElementXMiddle = xLayerIndent + xElementLeftPadding + elementWidth/2.0;
        for(uint64 k = 0; k<_numElementsBeforeDots; ++k)
        {
            _elementXMid[k] = firstElementXMiddle + k * elementWidthPlusSpace;
        }

        // elements represented by dots
        double dotsLeft = xLayerIndent + xElementLeftPadding + _numElementsBeforeDots * elementWidthPlusSpace;
        double dotsRight = xLayerIndent + _layerWidth - xElementRightPadding - elementWidthPlusSpace;
        _dotsXMid = (dotsLeft + dotsRight) / 2.0;
        double numInvisibleElements = double(numElements - _numElementsBeforeDots - 1);
        double invisibleElementsSpacing = (dotsRight - dotsLeft) / (numInvisibleElements-1);

        for(uint64 k = _numElementsBeforeDots; k<numElements-1; ++k)
        {
            _elementXMid[k] = dotsLeft + (k- _numElementsBeforeDots) * invisibleElementsSpacing;
        }

        /// element after dots
        _elementXMid[numElements-1] = dotsRight + xElementSpacing + elementWidth/2.0;
    }
}

double ElementXLayout::GetXMid(uint64 index) const
{
    return _elementXMid[index];
}

double ElementXLayout::GetWidth() const
{
    return _layerWidth;
}

bool ElementXLayout::IsAbbreviated() const
{
    return _abbreviated;
}

uint64 ElementXLayout::GetNumElementsBeforeDots() const
{
    return _numElementsBeforeDots;
}

double ElementXLayout::GetDotsXMid() const
{
    return _dotsXMid;
}

