// SvgHelpers.h

#pragma once

#include "LayerLayout.h"

#include "types.h"

#include <iostream>
using std::ostream;

#include <string>
using std::string;

/// TODO
///
void SvgRect(ostream& os, uint64 numTabs, const string& SvgClass, double x, double y, double width, double height, double radius);

/// TODO
///
void SvgCircle(ostream& os, uint64 numTabs, const string& SvgClass, double cx, double cy, double radius);

/// TODO
///
void SvgText(ostream& os, uint64 numTabs, const string& SvgClass, double cx, double cy, string text, double rotate);

/// TODO
///
void SvgNumber(ostream& os, uint64 numTabs, const string& SvgClass, double cx, double cy, double number, int maxChars, double rotate);

/// TODO
///
void SvgUse(ostream& os, uint64 numTabs, string id, double x, double y);

/// TODO
///
void SvgDots(ostream& os, uint64 numTabs, double cx, double cy);

/// TODO
///
void SvgEdge(ostream & os, uint64 numTabs, Point from, Point to, double edgeFlattness);

///
///
void SvgValueElement(ostream & os, uint64 numTabs, double cx, double cy, double number, uint64 maxChars, uint64 index);

///
///
void SvgEmptyElement(ostream & os, uint64 numTabs, double cx, double cy, uint64 index);
