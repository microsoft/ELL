// svgHelpers.cpp

#include "svgHelpers.h"
#include "LayerLayout.h"

// utilites
#include "StringFormat.h"
using utilities::StringFormat;

// stl
#include <iomanip>
using std::setprecision;
using std::fixed;

using std::endl;

#include <string>
using std::to_string;

#include <cmath>

// unexposed utility functions
void svgTab(ostream& os, uint64 numTabs)
{
    for(uint64 i = 0; i<numTabs; ++i)
    {
        os << "    ";
    }
}

int GetPrecision(double number, int maxChars)
{
    int precision = 0;
    if(number >= 1)
    {
        precision = maxChars - 1 -(int)ceil(log10(number));   // save one char for "."
    }
    else if(number < 1 && number >= 0)
    {
        precision = maxChars-2;   // save two characters for "0."
    }
    else if(number < 0 && number > -1)
    {
        precision = maxChars - 3;   // save three chars for "-0."
    }
    else
    {
        precision = maxChars - 2 - (int)ceil(log10(-number));   // save two chars for "-" and "."
    }
    if(precision < 0)
    {
        precision = 0;
    }

    return precision;
}

void svgRect(ostream& os, uint64 numTabs, const char* svgClass, double x, double y, double width, double height, double radius)
{
    svgTab(os, numTabs);
    auto format = R"aw(<rect class="%s" x="%f" y="%f" width="%f" height="%f" rx="%f" ry="%f" />\n)aw";
    StringFormat(os, format, svgClass, x, y, width, height, radius, radius);
}

void svgCircle(ostream& os, uint64 numTabs, const char* svgClass, double cx, double cy, double radius)
{
    svgTab(os, numTabs);
    auto format = R"aw(<rect class="%s" cx="%f" cy="%f" rx="%f" ry="%f" />\n)aw";
    StringFormat(os, format, svgClass, cx, cy, radius, radius);
}

void svgText(ostream& os, uint64 numTabs, const char* svgClass, double cx, double cy, string text, double rotate)
{
    svgTab(os, numTabs);
    auto format = R"aw(<text class="%s" x="%f" y="%f" text-anchor="middle" dy=".4em"  transform="rotate(%f,%f,%f)">)aw";
    StringFormat(os, format, svgClass, cx, cy, rotate, cx, cy);
    os << text << "</text>\n";
}

void svgNumber(ostream& os, uint64 numTabs, const char* svgClass, double cx, double cy, double number, int maxChars, double rotate)
{
    stringstream ss;
    ss << fixed << setprecision(GetPrecision(number, maxChars)) << number;
    svgText(os, numTabs, svgClass, cx, cy, ss.str().c_str(), rotate);
}

void svgUse(ostream& os, string href, double x, double y)
{
    os << "    <use xlink:href=\"#" << href << "\" x=\"" << x << "\" y=\"" << y << "\" />\n";
}

void svgDots(ostream& os, double cx, double cy)
{
    //svgCircle(os, "Dots", cx-8, cy, 2);
    //svgCircle(os, "Dots", cx, cy, 2);
    //svgCircle(os, "Dots", cx+8, cy, 2);
}

void svgEdge(ostream & os, Point from, Point to, double edgeFlattness)
{
    double xDist = to.x - from.x;
    double yDist = to.y - from.y;

    if (xDist == 0)
    {
        os << "    <path class=\"Edge\" d=\"M " << from.x << " " << from.y << " L " << to.x << " " << to.y << "\"/>\n";
    }

    else
    {
        const double drop = yDist*edgeFlattness / 2.0;
        double qFrac = drop / xDist;
        if (qFrac < 0) qFrac = -qFrac;
        if (qFrac > 0.25) qFrac = 0.25;
        double slopeFrac = 1 - 2 * qFrac;

        const double slopeDy = yDist - 2.0 * drop;

        os << "    <path class=\"Edge\" d=\"M " << from.x << " " << from.y << " q 0 " << drop << " " << xDist*qFrac << " " << drop+slopeDy*qFrac << " l " << xDist*slopeFrac << " " << slopeDy*slopeFrac << " q " << xDist*qFrac << " " << slopeDy*qFrac << " " << xDist*qFrac << " " << drop+slopeDy*qFrac << "\"/>\n";
    }
}

string svgDefineElement(ostream& os, void* uid, ElementStyleArgs styleArgs)
{
    //string typeName = "Element" + to_string((uint64)uid);
    //os << "<defs>\n<g id = \"" << typeName << "\">\n";
    //svgCircle(os, "Connector", styleArgs.width/2.0, 0, styleArgs.connectorRadius);
    //svgCircle(os, "Connector", styleArgs.width/2.0, styleArgs.height, styleArgs.connectorRadius);
    //svgRect(os, "Element", 0, 0, styleArgs.cornerRadius, styleArgs.width, styleArgs.height);
    //os << "</g>\n</defs>\n\n";
    //return typeName;
    return "";
}

