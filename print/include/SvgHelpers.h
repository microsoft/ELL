////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SvgHelpers.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LayerLayout.h"

// types
#include "types.h"

// stl
#include <iostream>
#include <string>

/// TODO
///
void SvgRect(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double x, double y, double width, double height, double radius);

/// TODO
///
void SvgCircle(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double cx, double cy, double radius);

/// TODO
///
void SvgText(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double cx, double cy, const std::string& text, double rotate);

/// TODO
///
void SvgNumber(std::ostream& os, uint64 numTabs, const std::string& SvgClass, double cx, double cy, double number, int maxChars, double rotate);

/// TODO
///
void SvgUse(std::ostream& os, uint64 numTabs, const std::string& id, double x, double y);

/// TODO
///
void SvgDots(std::ostream& os, uint64 numTabs, double cx, double cy);

/// TODO
///
void SvgEdge(std::ostream & os, uint64 numTabs, Point from, Point to, double edgeFlattness);

///
///
void SvgValueElement(std::ostream & os, uint64 numTabs, double cx, double cy, double number, uint64 maxChars, uint64 index);

///
///
void SvgEmptyElement(std::ostream & os, uint64 numTabs, double cx, double cy, uint64 index);

///
///
void SvgInputElement(std::ostream & os, uint64 numTabs, double cx, double cy, uint64 index);
