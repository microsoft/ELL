////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SvgHelpers.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LayerLayout.h"

// stl
#include <cstdint>
#include <ostream>
#include <string>

/// <summary> Svg rectangle. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="SvgClass"> The svg class. </param>
/// <param name="x"> The x coordinate. </param>
/// <param name="y"> The y coordinate. </param>
/// <param name="width"> The width. </param>
/// <param name="height"> The height. </param>
/// <param name="radius"> The radius. </param>
void SvgRect(std::ostream& os, uint64_t numTabs, const std::string& SvgClass, double x, double y, double width, double height, double radius);

/// <summary> Svg circle. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="SvgClass"> The svg class. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
/// <param name="radius"> The radius. </param>
void SvgCircle(std::ostream& os, uint64_t numTabs, const std::string& SvgClass, double cx, double cy, double radius);

/// <summary> Svg text. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="SvgClass"> The svg class. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
/// <param name="text"> The text. </param>
/// <param name="rotate"> The rotate. </param>
void SvgText(std::ostream& os, uint64_t numTabs, const std::string& SvgClass, double cx, double cy, const std::string& text, double rotate);

/// <summary> Svg number. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="SvgClass"> The svg class. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
/// <param name="number"> Number of. </param>
/// <param name="maxChars"> The maximum characters. </param>
/// <param name="rotate"> The rotate. </param>
void SvgNumber(std::ostream& os, uint64_t numTabs, const std::string& SvgClass, double cx, double cy, double number, int maxChars, double rotate);

/// <summary> Add an Svg use statement. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="id"> The identifier. </param>
/// <param name="x"> The x coordinate. </param>
/// <param name="y"> The y coordinate. </param>
void SvgUse(std::ostream& os, uint64_t numTabs, const std::string& id, double x, double y);

/// <summary> Svg dots. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
void SvgDots(std::ostream& os, uint64_t numTabs, double cx, double cy);

/// <summary> Svg edge. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="from"> Source for the. </param>
/// <param name="to"> to. </param>
/// <param name="edgeFlattness"> The edge flattness. </param>
void SvgEdge(std::ostream& os, uint64_t numTabs, Point from, Point to, double edgeFlattness);

/// <summary> Svg value element. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
/// <param name="number"> Number of. </param>
/// <param name="maxChars"> The maximum characters. </param>
/// <param name="index"> Zero-based index of the. </param>
void SvgValueElement(std::ostream& os, uint64_t numTabs, double cx, double cy, double number, uint64_t maxChars, uint64_t index);

/// <summary> Svg empty element. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
/// <param name="index"> Zero-based index of the. </param>
void SvgEmptyElement(std::ostream& os, uint64_t numTabs, double cx, double cy, uint64_t index);

/// <summary> Svg input element. </summary>
///
/// <param name="os"> [in,out] Stream to write data to. </param>
/// <param name="numTabs"> Number of tabs. </param>
/// <param name="cx"> The cx. </param>
/// <param name="cy"> The cy. </param>
/// <param name="index"> Zero-based index of the. </param>
void SvgInputElement(std::ostream& os, uint64_t numTabs, double cx, double cy, uint64_t index);
