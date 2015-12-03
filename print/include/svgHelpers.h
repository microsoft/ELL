// SVGHelpers.h

#pragma once

#include "types.h"

#include <iostream>
using std::ostream;

#include <string>
using std::string;

void svgRect(ostream& os, string svgClass, double x, double y, double radius, double width, double height);
void svgCircle(ostream& os, string svgClass, double cx, double cy, double radius);
void svgText(ostream& os, string text, string svgClass, double cx, double y, bool vertical = false);
void svgText(ostream& os, double text, int precision, string svgClass, double cx, double y);
void svgUse(ostream& os, string href, double x, double y);

