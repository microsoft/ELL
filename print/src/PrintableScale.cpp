// PrintableScale.cpp

#include "PrintableScale.h"
#include "svgHelpers.h"

#include <memory>
using std::make_unique;

void PrintableScale::Print(ostream & os, uint64 index, const vector<shared_ptr<IPrintable>>& layers) const
{
   svgPrintLayer(os, index, "SCALE", "ScaleLayer", _values, _x, _cy, _layerCornerRadius, *_upLayout, _layerHeight, _elementWidth, _elementHeight, _elementCornerRadius, _elementConnectorRadius, _valueMaxChars);
}

void PrintableScale::ComputeLayout(const CommandLineArgs& args, double layerYOffset)
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

Point PrintableScale::GetBeginPoint(uint64 index) const
{
    return Point{_upLayout->GetXMid(index), 0} ;
}

Point PrintableScale::GetEndPoint(uint64 index) const
{
    return Point();
}

double PrintableScale::GetWidth() const
{
    return _upLayout->GetWidth();
}

double PrintableScale::GetHeight() const
{
    return _layerHeight;
}

