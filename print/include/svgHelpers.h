// SVGHelpers.h

#pragma once

#include "ElementXLayout.h"

#include "types.h"

#include <iostream>
using std::ostream;

#include <string>
using std::string;

#include <vector>
using std::vector;

void svgRect(ostream& os, string svgClass, double x, double y, double radius, double width, double height);
void svgCircle(ostream& os, string svgClass, double cx, double cy, double radius);
void svgText(ostream& os, string text, string svgClass, double cx, double y, bool vertical = false);
void svgText(ostream& os, double text, int precision, string svgClass, double cx, double y);
void svgDots(ostream& os, double cx, double cy);
void svgUse(ostream& os, string href, double x, double y);

int GetPrecision(double value, int maxChars);

string svgDefineElement(ostream& os, uint64 index, double width, double height, double cornerRadius, double connectorRadius);
