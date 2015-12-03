// svgHelpers.cpp

#include "svgHelpers.h"

#include <iomanip>
using std::setprecision;
using std::fixed;

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