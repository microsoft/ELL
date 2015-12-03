// PrintableScale.cpp

#include "PrintableScale.h"
#include "svgHelpers.h"

#include <memory>
using std::make_unique;

#include <string>
using std::to_string;
using std::string;

#include <iomanip>
using std::fixed;
using std::setprecision;

#include <cmath>
using std::log10;
using std::ceil;

int getPrecision(double x, int maxChars = 5)
{
    int digits = 0;
    if(x >= 1)
    {
        digits = (int)ceil(log10(x));
    }
    else if(x < 1 && x >= 0)
    {
        digits = 1;
    }
    else if (x < 0 && x > -1)
    {
        digits = 2;
    }
    else
    {
        digits = (int)ceil(log10(-x));
    }

    int precision = maxChars - 1 - digits;
    if(precision < 0)
    {
        return 0;
    }

    return precision;
}

void PrintableScale::Print(ostream & os, const vector<shared_ptr<IPrintable>>& layers) const
{
    double halfWidth = _elementWidth/2.0;
    double elementTop = _yLayerOffset + _yElementPadding;
    double layerWidth = _upLayout->GetWidth();
    double layerHeight = _elementHeight + _yElementPadding * 2.0;
    double yLayerMid = _yLayerOffset + _yElementPadding + _elementHeight/2.0;

    uint64 index = 1; // TODO

    // define what an element looks like
    string defName = "Element" + to_string((uint64)this);
    os << "<defs>\n<g id = \"" << defName << "\">\n";
    svgCircle(os, "Connector", halfWidth, 0, 5);
    svgCircle(os, "Connector", halfWidth, _elementHeight, 5);
    svgRect(os, "Element", 0, 0, 10, _elementWidth, _elementHeight);
    os << "</g>\n</defs>\n\n";

    // draw the layer rectangle
    svgRect(os, "ScaleLayer", _xLayerIndent, _yLayerOffset, 10, layerWidth, layerHeight);

    // write the layer index and type
    double indexLeft = _xLayerIndent + 15;
    double typeLeft = (_upLayout->GetXMid(0) - halfWidth + indexLeft) / 2.0;

    svgText(os, to_string(index), "Layer", indexLeft, yLayerMid);
    svgText(os, "SCALE", "Layer", typeLeft, yLayerMid, true);

    // 
    for(uint64 k=0; k< _upLayout->GetNumElementsBeforeDots() ; ++k)
    {
        double elementLeft = _upLayout->GetXMid(k) - halfWidth;
        double elementMid = _upLayout->GetXMid(k);
        svgUse(os, defName, elementLeft, elementTop);
        
        int precision = getPrecision(_values[k]);
        svgText(os, _values[k], precision, "Element", elementMid, yLayerMid);
    }

    if(_upLayout->IsAbbreviated())
    {
        uint64 k = _output.Size() - 1;
        double elementLeft = _upLayout->GetXMid(k) - halfWidth;
        double elementMid = _upLayout->GetXMid(k);

        os << "    <use xlink:href=\"#Element" << to_string((uint64)this) << "\" x=\"" << elementLeft << "\" y=\"" << elementTop << "\" />\n";
        int precision = getPrecision(_values[k]);
        svgText(os, _values[k], precision, "Element", elementMid, yLayerMid);

        uint64 dotsXMid = uint64(_upLayout->GetDotsXMid());

        os << "    <ellipse cx = \"" << dotsXMid-8 << "\" cy = \"" << yLayerMid << "\" rx = \"2\" ry = \"2\" />\n";
        os << "    <ellipse cx = \"" << dotsXMid << "\" cy = \"" << yLayerMid << "\" rx = \"2\" ry = \"2\" />\n";
        os << "    <ellipse cx = \"" << dotsXMid+8 << "\" cy = \"" << yLayerMid << "\" rx = \"2\" ry = \"2\" />\n";
    }
}

void PrintableScale::ComputeLayout(const CommandLineArgs& args, double layerYOffset)
{
    _upLayout = make_unique<ElementXLayout>(_output.size(), args);
    _xLayerIndent = args.xLayerIndent;
    _yLayerOffset = layerYOffset;
    _elementWidth = args.elementWidth;
    _elementHeight = args.elementHeight;
    _yElementPadding = args.yElementPadding;
}

Point PrintableScale::GetBeginPoint(uint64 index) const
{
    return Point();
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
    return 50;
}
