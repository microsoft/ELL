////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableLayer.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableLayer.h"
#include "SvgHelpers.h"

// stl
#include <string>
#include <algorithm>

std::string PrintableLayer::GetTypeName()
{
    return "PrintLayer";
}

LayerLayout PrintableLayer::PrintLayer(std::ostream& os, double left, double top, uint64_t layerIndex, const std::string& typeName, uint64_t size, const ElementLayoutArguments& elementLayout, const LayerStyleArguments& layerStyle)
{
    // calculate the layout
    LayerLayout layout(left, top, size, layerStyle.maxWidth, elementLayout);

    // draw the layer rectangle
    SvgRect(os, 2, typeName, left, top, layout.GetWidth(), layout.GetHeight(), layerStyle.cornerRadius);
    
    // print the layer index
    SvgText(os, 2, "Layer", left + 15, layout.GetMidY(), std::to_string(layerIndex), 0);

    // print the layer type
    std::string uppercaseTypeName = typeName;
    transform(typeName.cbegin(), typeName.cend(), uppercaseTypeName.begin(), ::toupper);
    if (uppercaseTypeName.size() > 5)
    {
        uppercaseTypeName = uppercaseTypeName.substr(0, 5);
    }
    SvgText(os, 2, "Layer", left + 40, layout.GetMidY(), uppercaseTypeName, -90);

    return layout;
}

void PrintableLayer::PrintEmptyElements(std::ostream& os, const LayerLayout& layout)
{
    //// print the visible elements, before the dots
    for (uint64_t k = 0; k < layout.NumVisibleElements() - 1; ++k)
    {
        SvgEmptyElement(os, 2, layout.GetMidX(k), layout.GetMidY(), k);
    }

    // print last element
    auto lastElementIndex = layout.NumElements() - 1;
    SvgEmptyElement(os, 2, layout.GetMidX(lastElementIndex), layout.GetMidY(), lastElementIndex);

    // if has hidden elements, draw the dots
    if (layout.HasHiddenElements())
    {
        SvgDots(os, 2, layout.GetDotsMidX(), layout.GetMidY());
    }
}

void PrintableLayer::PrintValueElements(std::ostream& os, const LayerLayout& layout, const std::vector<double>& values, uint64_t maxChars)
{
    //// print the visible elements, before the dots
    for (uint64_t k = 0; k < layout.NumVisibleElements() - 1; ++k)
    {
        SvgValueElement(os, 2, layout.GetMidX(k), layout.GetMidY(), values[k], maxChars, k);
    }

    // print last element
    auto lastElementIndex = layout.NumElements() - 1;
    SvgValueElement(os, 2, layout.GetMidX(lastElementIndex), layout.GetMidY(), values[lastElementIndex], maxChars, lastElementIndex);

    // if has hidden elements, draw the dots
    if (layout.HasHiddenElements())
    {
        SvgDots(os, 2, layout.GetDotsMidX(), layout.GetMidY());
    }
}
