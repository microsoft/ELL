////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SvgHelpers.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SvgHelpers.h"
#include "LayerLayout.h"

// utilites
#include "StringFormat.h"

// stl
#include <iomanip>
#include <string>
#include <cmath>

// unexposed utility functions
void SvgTab(std::ostream& os, uint64 numTabs)
{
    for(uint64 i = 0; i<numTabs; ++i)
    {
        os << "    ";
    }
}

int GetPrecision(double number, uint64 maxChars)
{
    int precision;
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

void SvgRect(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double x, double y, double width, double height, double radius)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<rect class="%s" x="%f" y="%f" width="%f" height="%f" rx="%f" ry="%f" />)aw";
    utilities::StringFormat(os, format, SvgClass, x, y, width, height, radius, radius);
    os << std::endl;
}

void SvgCircle(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double cx, double cy, double radius)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<ellipse class="%s" cx="%f" cy="%f" rx="%f" ry="%f" />)aw";
    utilities::StringFormat(os, format, SvgClass, cx, cy, radius, radius);
    os << std::endl;
}

void SvgText(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double cx, double cy, const std::string& text, double rotate)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<text class="%s" x="%f" y="%f" text-anchor="middle" dy=".4em"  transform="rotate(%f,%f,%f)">)aw";
    utilities::StringFormat(os, format, SvgClass, cx, cy, rotate, cx, cy);
    os << text << "</text>\n";
}

void SvgNumber(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double cx, double cy, double number, uint64 maxChars, double rotate)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(GetPrecision(number, maxChars)) << number;
    SvgText(os, numTabs, SvgClass, cx, cy, ss.str(), rotate);
}

void SvgUse(std::ostream& os, uint64 numTabs, const std::string& id, double x, double y)
{
    SvgTab(os, numTabs);
    auto format = R"aw(<use xlink:href="#%s" x="%f" y="%f" />)aw";
    utilities::StringFormat(os, format, id, x, y);
    os << std::endl;
}

void SvgDots(std::ostream& os, uint64 numTabs, double cx, double cy)
{
    SvgCircle(os, numTabs, "Dots", cx-8, cy, 2);
    SvgCircle(os, numTabs, "Dots", cx, cy, 2);
    SvgCircle(os, numTabs, "Dots", cx+8, cy, 2);
}

void SvgEdge(std::ostream & os, uint64 numTabs, Point from, Point to, double edgeFlattness)
{
    double xDist = to.x - from.x;
    double yDist = to.y - from.y;

    if (xDist == 0)
    {
        SvgTab(os, numTabs);
        const char* format = R"aw(<path class="Edge" d="M %f %f L %f %f" />)aw";
        utilities::StringFormat(os, format, from.x, from.y, to.x, to.y);
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
        utilities::StringFormat(os, format, 
            from.x, from.y, 
            drop, xDist*qFrac, drop+slopeDy*qFrac, 
            xDist*slopeFrac, slopeDy*slopeFrac, 
            xDist*qFrac, slopeDy*qFrac, xDist*qFrac, drop+slopeDy*qFrac);
    }

    os << std::endl;
}

void SvgValueElement(std::ostream& os, uint64 numTabs, double cx, double cy, double number, uint64 maxChars, uint64 index)
{
    SvgUse(os, 2, "ValueElement", cx, cy);
    SvgNumber(os, 2, "Element", cx, cy-5, number, maxChars, 0);
    SvgText(os, 2, "ElementIndex", cx, cy+10, std::to_string(index), 0);
}

void SvgEmptyElement(std::ostream& os, uint64 numTabs, double cx, double cy, uint64 index)
{
    SvgUse(os, 2, "EmptyElement", cx, cy);
    SvgText(os, 2, "ElementIndex", cx, cy, std::to_string(index), 0);
}

void SvgInputElement(std::ostream& os, uint64 numTabs, double cx, double cy, uint64 index)
{
    SvgUse(os, 2, "InputElement", cx, cy);
    SvgText(os, 2, "ElementIndex", cx, cy, std::to_string(index), 0);
}
