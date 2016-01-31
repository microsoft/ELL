// SVGHelpers.h

#pragma once

#include "LayerLayout.h"

#include "types.h"

#include <iostream>
using std::ostream;

#include <string>
using std::string;

/// TODO
///
void svgRect(ostream& os, uint64 numTabs, const string& svgClass, double x, double y, double width, double height, double radius);

/// TODO
///
void svgCircle(ostream& os, uint64 numTabs, const string& svgClass, double cx, double cy, double radius);

/// TODO
///
void svgText(ostream& os, uint64 numTabs, const string& svgClass, double cx, double cy, string text, double rotate);

/// TODO
///
void svgNumber(ostream& os, uint64 numTabs, const string& svgClass, double cx, double cy, double number, int maxChars, double rotate);

/// TODO
///
void svgUse(ostream& os, uint64 numTabs, string id, double x, double y);

/// TODO
///
void svgDots(ostream& os, uint64 numTabs, double cx, double cy);

/// TODO
///
void svgEdge(ostream & os, uint64 numTabs, Point from, Point to, double edgeFlattness);

///
///
void svgValueElement(ostream & os, uint64 numTabs, double cx, double cy, double number, int maxChars, uint64 index);