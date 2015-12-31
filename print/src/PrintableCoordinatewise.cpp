// PrintableCoordinatewise.cpp

#include "PrintableCoordinatewise.h"
#include "svgHelpers.h"

using std::make_unique;

#include <string>
using std::to_string;

PrintableCoordinatewise::PrintableCoordinatewise(string typeName) : _typeName(typeName)
{}

uint64 PrintableCoordinatewise::Size() const
{
    return Coordinatewise::Size();
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
       int precision = GetPrecision(value, _valueElementMaxChars);

       svgUse(os, elementDefName, elementLeft, elementTop);
       svgText(os, value, precision, "Element", elementXMid, _cy - 5);
       svgText(os, to_string(k), "ElementIndex", elementXMid, _cy + 10);

       auto input = _coordinates[k];
   }

   // if has hidden elements, draw the dots
   if(_upLayout->HasHidden())
   {
       double dotsXMid = _upLayout->GetGapXMid();
       svgDots(os, dotsXMid, _cy);
   }
}

void PrintableCoordinatewise::ComputeLayout(const CommandLineArguments& args, double layerLeft, double layerTop)
{
    _upLayout = make_unique<HorizontalLayout>(_values.size(), layerLeft, args.layerMaxWidth, args.valueElementStyle.width, args.valueElementStyle.horizontalSpacing, args.valueElementStyle.leftPadding, args.valueElementStyle.rightPadding, args.valueElementStyle.dotsWidth); // just pass the entire tyle struct

    _layerHeight = args.valueElementStyle.height + 2*args.valueElementStyle.verticalPadding;
    _cy = layerTop + _layerHeight / 2.0;   
    _elementStyle = args.valueElementStyle;
    _valueElementMaxChars = args.valueElementMaxChars;
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

