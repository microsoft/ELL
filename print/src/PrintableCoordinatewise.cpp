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

void PrintableCoordinatewise::Print(ostream & os) const
{
   double elementTop = _cy - _elementStyle.height/2.0;
   double elementBottom = elementTop + _elementStyle.height;

   // define the element shape
   string elementDefName = svgDefineElement(os, (void*)this, _elementStyle);

   // print the visible elements, before the dots
   for (uint64 k = 0; k < _values.size(); ++k)
   {
       if (_upLayout->IsHidden(k))
       {
           continue;
       }

       double elementXMid = _upLayout->GetXMid(k);
       double elementLeft = elementXMid- _elementStyle.width / 2.0;
       double value = _values[k];
       int precision = GetPrecision(value, _valueMaxChars);
       printElement(os, elementDefName, k, elementXMid, elementLeft, _cy, elementTop, value, precision);
       auto input = _coordinates[k];
   }

   // if has hidden elements, draw the dots
   if(_upLayout->HasHidden())
   {
       double dotsXMid = _upLayout->GetGapXMid();
       svgDots(os, dotsXMid, _cy);
   }
}

void PrintableCoordinatewise::ComputeLayout(const CommandLineArgs& args, double layerYOffset)
{
    _upLayout = make_unique<ElementXLayout>(_values.size(), args.xLayerIndent, args.maxLayerWidth, args.coordinatewiseElementStyle.width, args.xElementSpacing, args.xElementLeftPadding, args.xElementRightPadding, args.dotsWidth);

    _layerHeight = args.coordinatewiseElementStyle.height + 2*args.yElementPadding;
    _cy = layerYOffset + _layerHeight / 2.0;   
    _elementStyle = args.coordinatewiseElementStyle;
    _valueMaxChars = args.valueMaxChars;
    _edgeFlattness = args.edgeFlattness;
}

Point PrintableCoordinatewise::GetBeginPoint(uint64 index) const
{
    return Point{_upLayout->GetXMid(index), _cy + (_elementStyle.height + _elementStyle.connectorRadius)/2.0} ;
}

Point PrintableCoordinatewise::GetEndPoint(uint64 index) const
{
    return Point{_upLayout->GetXMid(index), _cy - (_elementStyle.height + _elementStyle.connectorRadius) / 2.0};
}

double PrintableCoordinatewise::GetWidth() const
{
    return _upLayout->GetWidth();
}

double PrintableCoordinatewise::GetHeight() const
{
    return _layerHeight;
}

bool PrintableCoordinatewise::IsHidden(uint64 index) const
{
    return _upLayout->IsHidden(index);
}

string PrintableCoordinatewise::GetTypeName() const
{
    return _typeName;
}

