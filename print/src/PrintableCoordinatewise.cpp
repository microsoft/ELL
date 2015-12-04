// PrintableCoordinatewise.cpp

#include "PrintableCoordinatewise.h"
#include "svgHelpers.h"

#include <memory>
using std::make_unique;

PrintableCoordinatewise::PrintableCoordinatewise(string typeName, string svgClass) : _typeName(typeName), _svgClass(svgClass)
{}

void PrintableCoordinatewise::Print(ostream & os, uint64 index, const vector<shared_ptr<IPrintable>>& layers) const
{
   svgPrintLayer(os, index, _typeName, _svgClass, _values, _x, _cy, _layerCornerRadius, *_upLayout, _layerHeight, _elementWidth, _elementHeight, _elementCornerRadius, _elementConnectorRadius, _valueMaxChars);
}

void PrintableCoordinatewise::ComputeLayout(const CommandLineArgs& args, double layerYOffset)
{
    _upLayout = make_unique<ElementXLayout>(_values.size(), args);

    _x = args.xLayerIndent;
    _layerCornerRadius = args.layerCornerRadius;
    _layerHeight = args.elementHeight + 2*args.yElementPadding;
    _cy = layerYOffset + _layerHeight / 2.0;
    _elementWidth = args.elementWidth;
    _elementHeight = args.elementHeight;
    _elementCornerRadius = args.elementCornerRadius;
    _elementConnectorRadius = args.elementConnectorRadius;
    _valueMaxChars = args.valueMaxChars;
}

Point PrintableCoordinatewise::GetBeginPoint(uint64 index) const
{
    return Point{_upLayout->GetXMid(index), 0} ;
}

Point PrintableCoordinatewise::GetEndPoint(uint64 index) const
{
    return Point();
}

double PrintableCoordinatewise::GetWidth() const
{
    return _upLayout->GetWidth();
}

double PrintableCoordinatewise::GetHeight() const
{
    return _layerHeight;
}

