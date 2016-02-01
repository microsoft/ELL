// SvgHelpers.cpp

#include "SvgHelpers.h"
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
void SvgTab(ostream& os, uint64 numTabs)
{
    for(uint64 i = 0; i<numTabs; ++i)
    {
        os << "    ";
    }
}

int GetPrecision(double number, uint64 maxChars)
{
    int precision = 0;
    if(number >= 1)
    {
        precision = (int)maxChars - 1 -(int)ceil(log10(number));   // save one char for "."
    }
    else if(number < 1 && number >= 0)
    {
        precision = (int)maxChars-2;   // save two characters for "0."
    }
    else if(number < 0 && number > -1)
    {
        precision = (int)maxChars - 3;   // save three chars for "-0."
    }
    else
    {
        precision = (int)maxChars - 2 - (int)ceil(log10(-number));   // save two chars for "-" and "."
    }
    if(precision < 0)
    {
        precision = 0;
    }

    return precision;
}

void SvgRect(ostream& os, uint64 numTabs, const string& SvgClass, double x, double y, double width, double height, double radius)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<rect class="%s" x="%f" y="%f" width="%f" height="%f" rx="%f" ry="%f" />)aw";
    StringFormat(os, format, SvgClass, x, y, width, height, radius, radius);
    os << endl;
}

void SvgCircle(ostream& os, uint64 numTabs, const string& SvgClass, double cx, double cy, double radius)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<ellipse class="%s" cx="%f" cy="%f" rx="%f" ry="%f" />)aw";
    StringFormat(os, format, SvgClass, cx, cy, radius, radius);
    os << endl;
}

void SvgText(ostream& os, uint64 numTabs, const string& SvgClass, double cx, double cy, string text, double rotate)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<text class="%s" x="%f" y="%f" text-anchor="middle" dy=".4em"  transform="rotate(%f,%f,%f)">)aw";
    StringFormat(os, format, SvgClass, cx, cy, rotate, cx, cy);
    os << text << "</text>\n";
}

void SvgNumber(ostream& os, uint64 numTabs, const string& SvgClass, double cx, double cy, double number, uint64 maxChars, double rotate)
{
    stringstream ss;
    ss << fixed << setprecision(GetPrecision(number, maxChars)) << number;
    SvgText(os, numTabs, SvgClass, cx, cy, ss.str(), rotate);
}

void SvgUse(ostream& os, uint64 numTabs, string id, double x, double y)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<use xlink:href="#%s" x="%f" y="%f" />)aw";
    StringFormat(os, format, id, x, y);
    os << endl;
}

void SvgDots(ostream& os, uint64 numTabs, double cx, double cy)
{
    SvgCircle(os, numTabs, "Dots", cx-8, cy, 2);
    SvgCircle(os, numTabs, "Dots", cx, cy, 2);
    SvgCircle(os, numTabs, "Dots", cx+8, cy, 2);
}

void SvgEdge(ostream & os, uint64 numTabs, Point from, Point to, double edgeFlattness)
{
    double xDist = to.x - from.x;
    double yDist = to.y - from.y;

    if (xDist == 0)
    {
        SvgTab(os, numTabs);
        const char* format = R"aw(<path class="Edge" d="M %f %f L %f %f" />)aw";
        StringFormat(os, format, from.x, from.y, to.x, to.y);
    }

    else
    {
        const double drop = yDist*edgeFlattness / 2.0;
        double qFrac = drop / xDist;
        if (qFrac < 0) qFrac = -qFrac;
        if (qFrac > 0.25) qFrac = 0.25;
        double slopeFrac = 1 - 2 * qFrac;

        const double slopeDy = yDist - 2.0 * drop;

        SvgTab(os, numTabs);
        const char* format = R"aw(<path class="Edge" d="M %f %f q 0 %f %f %f l %f %f q %f %f %f %f" />)aw";
        StringFormat(os, format, 
            from.x, from.y, 
            drop, xDist*qFrac, drop+slopeDy*qFrac, 
            xDist*slopeFrac, slopeDy*slopeFrac, 
            xDist*qFrac, slopeDy*qFrac, xDist*qFrac, drop+slopeDy*qFrac);
    }

    os << endl;
}

void SvgValueElement(ostream & os, uint64 numTabs, double cx, double cy, double number, uint64 maxChars, uint64 index)
{
    SvgUse(os, 2, "ValueElement", cx, cy);
    SvgNumber(os, 2, "Element", cx, cy-5, number, maxChars, 0);
    SvgText(os, 2, "ElementIndex", cx, cy+10, to_string(index), 0);
}

void SvgEmptyElement(ostream & os, uint64 numTabs, double cx, double cy, uint64 index)
{
    SvgUse(os, 2, "EmptyElement", cx, cy);
    SvgText(os, 2, "ElementIndex", cx, cy, to_string(index), 0);
}
