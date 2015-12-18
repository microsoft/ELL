// PrintableInput.cpp

#include "PrintableInput.h"
#include "svgHelpers.h"

using std::make_unique;

#include <string>
using std::to_string;

inline void printElement(ostream & os, const string& elementDefName, uint64 index, double elementXMid, double elementLeft, double elementYMid, double elementTop)
{
    svgUse(os, elementDefName, elementLeft, elementTop);
    svgText(os, to_string(index), "ElementIndex", elementXMid, elementYMid);
}

void PrintableInput::Print(ostream & os, uint64 index, const vector<shared_ptr<IPrintable>>& layers) const
{
    double halfWidth = _elementStyle.width/2.0;
    double elementTop = _cy - _elementStyle.height/2.0;
    double elementBottom = elementTop + _elementStyle.height;

    // define the element shape
    string elementDefName = svgDefineElement(os, index, _elementStyle);

    // print the visible elements, before the dots
    for(uint64 k = 0; k< _upLayout->GetNumElementsBeforeDots(); ++k)
    {
        double elementXMid = _upLayout->GetXMid(k);
        double elementLeft = elementXMid-halfWidth;
        printElement(os, elementDefName, k, elementXMid, elementLeft, _cy, elementTop);
    }

    // if abbreviated, draw the dots and the last element
    if(_upLayout->IsAbbreviated())
    {
        uint64 k = _output.size()-1;
        double elementXMid = _upLayout->GetXMid(k);
        double elementLeft = elementXMid-halfWidth;
        printElement(os, elementDefName, k, elementXMid, elementLeft, _cy, elementTop);

        double dotsXMid = _upLayout->GetDotsXMid();
        svgDots(os, dotsXMid, _cy);
    }
}

void PrintableInput::ComputeLayout(const CommandLineArgs& args, double layerYOffset)
{
    _upLayout = make_unique<ElementXLayout>(_output.size(), args.xLayerIndent, args.maxLayerWidth, args.emptyElementStyle.width, args.xElementSpacing, args.xElementLeftPadding, args.xElementRightPadding, args.dotsWidth);

    _layerHeight = args.emptyElementStyle.height + 2*args.yEmptyElementPadding;
    _cy = layerYOffset + _layerHeight / 2.0;
    _elementStyle = args.emptyElementStyle;
}

Point PrintableInput::GetBeginPoint(uint64 index) const
{
    return Point{ _upLayout->GetXMid(index), _cy + (_elementStyle.height + _elementStyle.connectorRadius) / 2.0 };
}

Point PrintableInput::GetEndPoint(uint64 index) const
{
    return Point{ _upLayout->GetXMid(index), _cy - (_elementStyle.height + _elementStyle.connectorRadius) / 2.0 };
}

double PrintableInput::GetWidth() const
{
    return _upLayout->GetWidth();
}

double PrintableInput::GetHeight() const
{
    return _layerHeight;
}

string PrintableInput::GetTypeName() const
{
    return string("IN");
}
