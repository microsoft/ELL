// PrintableScale.cpp

#include "PrintableScale.h"

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
    uint64 halfWidth = uint64(_elementWidth/2.0);
    uint64 width = uint64(_elementWidth);
    uint64 height = uint64(_elementHeight);
    uint64 elementTop = uint64(_yLayerOffset + _yElementPadding);
    uint64 xLayerIndext = uint64(_xLayerIndent);
    uint64 yLayerOffset = uint64(_yLayerOffset);
    uint64 layerWidth = uint64(_upLayout->GetWidth());
    uint64 layerHeight = uint64(_elementHeight + _yElementPadding * 2.0);
    uint64 yLayerMid = uint64(_yLayerOffset + _yElementPadding + _elementHeight/2.0);

    uint64 index = 1; // TODO

    // define what an element looks like
    os << "<defs>\n";
    os << "    <g id = \"Element" << to_string((uint64)this)  << "\">\n";
    os << "        <ellipse class = \"Connector\" cx = \"" << halfWidth << "\" cy = \"0\" rx = \"5\" ry = \"5\" />\n";
    os << "        <ellipse class = \"Connector\" cx = \"" << halfWidth << "\" cy = \"" << height << "\" rx = \"5\" ry = \"5\" />\n";
    os << "        <rect class = \"Element\" x = \"0\" y = \"0\" rx = \"10\" ry = \"10\" width = \"" << width << "\" height = \"" << height << "\"/>\n";
    os << "    </g>\n";
    os << "</defs>\n\n";

    // draw the layer rectangle
    os << "    <rect class=\"ScaleLayer\" x=\"" << xLayerIndext << "\" y=\"" << yLayerOffset << "\" rx=\"10\" ry=\"10\" width=\"" << layerWidth << "\" height=\"" << layerHeight << "\"/>\n";

    // write the layer index and type
    uint64 layerIndexLeft = uint64(_xLayerIndent + 15);
    uint64 typeLeft = uint64((_upLayout->GetXMid(0) - halfWidth + _xLayerIndent + 30) / 2.0);

    os << "    <text x=\"" << layerIndexLeft << "\" y=\"" << yLayerMid+4 << "\" text-anchor=\"middle\" fill = \"white\" font-size = \"15\" font-weight = \"bold\">" << index << "</text>\n";
    os << "    <text x=\"" << typeLeft << "\" y=\"" << yLayerMid << "\" text-anchor=\"middle\" fill = \"white\" font-size = \"13\" transform=\"rotate(-90, " << typeLeft << ", " << yLayerMid << ")\">SCALE</text>\n";

    // 
    for(uint64 k=0; k< _upLayout->GetNumElementsBeforeDots() ; ++k)
    {
        uint64 elementLeft = uint64(_upLayout->GetXMid(k) - halfWidth);
        uint64 elementMid = uint64(_upLayout->GetXMid(k));
        os << "    <use xlink:href=\"#Element" << to_string((uint64)this) << "\" x=\"" << elementLeft << "\" y=\"" << elementTop << "\" />\n";
        
        int precision = getPrecision(_values[k]);
        os << "    <text x=\"" << elementMid << "\" y=\"" << yLayerMid+4 << "\" text-anchor=\"middle\" font-size = \"13\">" << fixed << setprecision(precision) <<  _values[k] << "</text>\n";
    }

    if(_upLayout->IsAbbreviated())
    {
        os << "    <use xlink:href=\"#Element" << to_string((uint64)this) << "\" x=\"" << uint64(_upLayout->GetXMid(_output.Size()-1) - halfWidth) << "\" y=\"" << elementTop << "\" />\n";

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
