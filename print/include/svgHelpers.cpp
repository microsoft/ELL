// svgHelpers.cpp

#include "svgHelpers.h"
#include "ElementXLayout.h"

#include <iomanip>
using std::setprecision;
using std::fixed;

#include <string>
using std::to_string;

void svgRect(ostream& os, string svgClass, double x, double y, double radius, double width, double height)
{
    os << "    <rect class=\"" << svgClass << "\" x=\"" << (int)x << "\" y=\"" << (int)y << "\" rx=\"" << (int)radius << "\" ry=\"" << (int)radius << "\" width=\"" << (int)width << "\" height=\"" << (int)height << "\"/>\n";
}

void svgCircle(ostream& os, string svgClass, double cx, double cy, double radius)
{
    os << "    <ellipse class = \"" << svgClass << "\" cx = \"" << (int)cx << "\" cy = \"" << (int)cy << "\" rx = \"" << (int)radius << "\" ry = \"" << (int)radius << "\" />\n";
}

void svgText(ostream& os, string text, string svgClass, double cx, double y, bool vertical)
{
    os << "    <text class=\"" << svgClass << "\" x=\"" << cx << "\" y=\"" << y << "\" text-anchor=\"middle\" dy=\".4em\"";
    if (vertical)
    {
        os << " transform=\"rotate(-90, " << cx << ", " << y << ")\"";
    }
    os << ">" << text << "</text>\n";
}

void svgText(ostream& os, double text, int precision, string svgClass, double cx, double y)
{
    os << "    <text class=\"" << svgClass << "\" x=\"" << cx << "\" y=\"" << y << "\" text-anchor=\"middle\" dy=\".4em\"" << ">" << fixed << setprecision(precision) << text << "</text>\n";
}

void svgUse(ostream& os, string href, double x, double y)
{
    os << "    <use xlink:href=\"#" << href << "\" x=\"" << x << "\" y=\"" << y << "\" />\n";
}

string svgDefineElement(ostream& os, uint64 index, double width, double height, double cornerRadius, double connectorRadius)
{
    string defName = "Element" + to_string(index);
    os << "<defs>\n<g id = \"" << defName << "\">\n";
    svgCircle(os, "Connector", width/2.0, 0, connectorRadius);
    svgCircle(os, "Connector", width/2.0, height, connectorRadius);
    svgRect(os, "Element", 0, 0, cornerRadius, width, height);
    os << "</g>\n</defs>\n\n";
    return defName;
}

void svgPrintLayer(ostream & os, uint64 index, const string& layerType, const string& layerSvgClass, const vector<double>& values, double x, double cy, double layerCornerRadius, const ElementXLayout& layout, double layerHeight, double elementWidth, double elementHeight, double elementCornerRadius, double elementConnectorRadius, int valueMaxChars)
{
    double halfWidth = elementWidth/2.0;
    double layerTop = cy - layerHeight/2.0;
    double elementTop = cy - elementHeight/2.0;
    double elementBottom = elementTop + elementHeight;
    double layerYMid = elementTop + elementHeight/2.0;

    // define the element shape
    string defName = svgDefineElement(os, index, elementWidth, elementHeight, elementCornerRadius, elementConnectorRadius);

    // draw the layer rectangle
    svgRect(os, layerSvgClass, x, layerTop, layerCornerRadius, layout.GetWidth(), layerHeight);

    // write the layer index and type
    double indexLeft = x + 15;
    double typeLeft = (layout.GetXMid(0) - halfWidth + indexLeft) / 2.0;
    svgText(os, to_string(index), "Layer", indexLeft, layerYMid);
    svgText(os, layerType, "Layer", typeLeft, layerYMid, true);

    // print the visible elements, before the dots
    for(uint64 k = 0; k< layout.GetNumElementsBeforeDots(); ++k)
    {
        svgPrintElement(os, values[k], valueMaxChars, k, defName, layout.GetXMid(k)-halfWidth, elementTop, elementWidth, elementHeight);
    }

    // if abbreviated, draw the dots and the last element
    if(layout.IsAbbreviated())
    {
        uint64 k = values.size()-1;
        svgPrintElement(os, values[k], valueMaxChars, k, defName, layout.GetXMid(k)-halfWidth, elementTop, elementWidth, elementHeight);

        double dotsXMid = layout.GetDotsXMid();

        svgCircle(os, "Dots", dotsXMid-8, layerYMid, 2);
        svgCircle(os, "Dots", dotsXMid, layerYMid, 2);
        svgCircle(os, "Dots", dotsXMid+8, layerYMid, 2);
    }
}

void svgPrintElement(ostream & os, double value, int maxChars, uint64 k, const string& defName, double elementLeft, double elementTop, double elementWidth, double elementHeight)
{
    svgUse(os, defName, elementLeft, elementTop);

    int precision = 0;
    if(value >= 1)
    {
        precision = maxChars - 1 -(int)ceil(log10(value));   // save one char for "."
    }
    else if(value < 1 && value >= 0)
    {
        precision = maxChars-2;   // save two characters for "0."
    }
    else if(value < 0 && value > -1)
    {
        precision = maxChars - 3;   // save three chars for "-0."
    }
    else
    {
        precision = maxChars - 2 - (int)ceil(log10(-value));   // save two chars for "-" and "."
    }
    if(precision < 0)
    {
        precision = 0;
    }

    double elementMid = elementLeft + elementWidth/2.0;
    svgText(os, value, precision, "Element", elementMid, elementTop + elementHeight/2.0 -4);
    svgText(os, to_string(k), "ElementIndex", elementMid, elementTop + elementHeight - 10);
}
