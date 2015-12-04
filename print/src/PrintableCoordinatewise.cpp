// PrintableCoordinatewise.cpp

#include "PrintableCoordinatewise.h"
#include "svgHelpers.h"

using std::make_unique;

#include <string>
using std::to_string;

PrintableCoordinatewise::PrintableCoordinatewise(string typeName) : _typeName(typeName)
{}

inline void printElement(ostream & os, const string& elementDefName, uint64 index, double elementXMid, double elementLeft, double elementYMid, double elementTop, double value, int precision)
{
    svgUse(os, elementDefName, elementLeft, elementTop);
    svgText(os, value, precision, "Element", elementXMid, elementYMid - 5);
    svgText(os, to_string(index), "ElementIndex", elementXMid, elementYMid + 10);
}

void PrintableCoordinatewise::Print(ostream & os, uint64 index, const vector<shared_ptr<IPrintable>>& layers) const
{
   double halfWidth = _elementWidth/2.0;
   double elementTop = _cy - _elementHeight/2.0;
   double elementBottom = elementTop + _elementHeight;

   // define the element shape
   string elementDefName = svgDefineElement(os, index, _elementWidth, _elementHeight, _elementCornerRadius, _elementConnectorRadius);

   // print the visible elements, before the dots
   for(uint64 k = 0; k< _upLayout->GetNumElementsBeforeDots(); ++k)
   {
       double elementXMid = _upLayout->GetXMid(k);
       double elementLeft = elementXMid-halfWidth;
       double value = _values[k];
       int precision = GetPrecision(value, _valueMaxChars);
       printElement(os, elementDefName, k, elementXMid, elementLeft, _cy, elementTop, value, precision);
   }

   // if abbreviated, draw the dots and the last element
   if(_upLayout->IsAbbreviated())
   {
       uint64 k = _values.size()-1;
       double elementXMid = _upLayout->GetXMid(k);
       double elementLeft = elementXMid-halfWidth;
       double value = _values[k];
       int precision = GetPrecision(value, _valueMaxChars);
       printElement(os, elementDefName, k, elementXMid, elementLeft, _cy, elementTop, value, precision);

       double dotsXMid = _upLayout->GetDotsXMid();
       svgDots(os, dotsXMid, _cy);
   }
}

void PrintableCoordinatewise::ComputeLayout(const CommandLineArgs& args, double layerYOffset)
{
    _upLayout = make_unique<ElementXLayout>(_values.size(), args.xLayerIndent, args.maxLayerWidth, args.elementWidth, args.xElementSpacing, args.xElementLeftPadding, args.xElementRightPadding, args.dotsWidth);

    _layerHeight = args.elementHeight + 2*args.yElementPadding;
    _cy = layerYOffset + _layerHeight / 2.0;
    _elementWidth = args.elementWidth;
    _elementHeight = args.elementHeight;
    _elementCornerRadius = args.elementCornerRadius;
    _elementConnectorRadius = args.elementConnectorRadius;
    _valueMaxChars = args.valueMaxChars;
    _endPointY = layerYOffset + args.yElementPadding - _elementConnectorRadius/2.0;
    _beginPointY = layerYOffset + args.yElementPadding + _elementHeight + _elementConnectorRadius/2.0;
}

Point PrintableCoordinatewise::GetBeginPoint(uint64 index) const
{
    return Point{_upLayout->GetXMid(index), _beginPointY} ;
}

Point PrintableCoordinatewise::GetEndPoint(uint64 index) const
{
    return Point{_upLayout->GetXMid(index), _endPointY};
}

double PrintableCoordinatewise::GetWidth() const
{
    return _upLayout->GetWidth();
}

double PrintableCoordinatewise::GetHeight() const
{
    return _layerHeight;
}

string PrintableCoordinatewise::GetTypeName() const
{
    return _typeName;
}

