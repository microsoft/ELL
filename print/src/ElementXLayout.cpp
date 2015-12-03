// ElementXLayout.cpp

#include "ElementXLayout.h"

ElementXLayout::ElementXLayout(uint64 numElements, const CommandLineArgs& args) : _elementXMid(numElements)
{
    
    // width of the layer without abbreviated format 
    double layerWidth = args.xElementLeftPadding + args.xElementRightPadding + numElements * args.elementWidth + (numElements-1) * args.xElementSpacing;
    double elementWidthPlusSpace = args.elementWidth + args.xElementSpacing;

    // non-abbreviated
    if(layerWidth <= args.maxLayerWidth)
    {
        _layerWidth = layerWidth;
        _numElementsBeforeDots = numElements;
        double firstElementXMiddle = args.xLayerIndent + args.xElementLeftPadding + args.elementWidth/2.0;
        for(uint64 k = 0; k<numElements; ++k)
        {
            _elementXMid[k] = firstElementXMiddle + k * elementWidthPlusSpace;
        }
    }
    
    //abbreviated
    else
    {
        _layerWidth = args.maxLayerWidth;
        _abbreviated = true;

        double elementsWidth = args.maxLayerWidth - args.xElementLeftPadding - args.xElementRightPadding;
        double elementsBeforeDotsWidth = elementsWidth - args.dotsWidth - args.elementWidth - args.xElementSpacing;
        _numElementsBeforeDots = uint64(elementsBeforeDotsWidth / (args.elementWidth + args.xElementSpacing));
        
        if(elementsBeforeDotsWidth <= 0 || _numElementsBeforeDots == 0)
        {
            throw runtime_error("unable to visualize layer within the specified constraints (increase width, decrease dots width or element width/spacing)");
        }

        // elements before dots
        double firstElementXMiddle = args.xLayerIndent + args.xElementLeftPadding + args.elementWidth/2.0;
        for(uint64 k = 0; k<_numElementsBeforeDots; ++k)
        {
            _elementXMid[k] = firstElementXMiddle + k * elementWidthPlusSpace;
        }

        // elements represented by dots
        double dotsLeft = args.xLayerIndent + args.xElementLeftPadding + _numElementsBeforeDots * elementWidthPlusSpace;
        double dotsRight = args.xLayerIndent + _layerWidth - args.xElementRightPadding - elementWidthPlusSpace;
        _dotsXMid = (dotsLeft + dotsRight) / 2.0;
        double numInvisibleElements = double(numElements - _numElementsBeforeDots - 1);
        double invisibleElementsSpacing = (dotsRight - dotsLeft) / (numInvisibleElements-1);

        for(uint64 k = _numElementsBeforeDots; k<numElements-1; ++k)
        {
            _elementXMid[k] = dotsLeft + (k- _numElementsBeforeDots) * invisibleElementsSpacing;
        }

        /// element after dots
        _elementXMid[numElements-1] = dotsRight + args.xElementSpacing + args.elementWidth/2.0;
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

