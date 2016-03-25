////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     PrintableLayer.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
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

LayerLayout PrintableLayer::Print(std::ostream& os, double left, double top, uint64_t layerIndex, const std::string& typeName, uint64_t size, const ElementLayoutArguments& elementLayout, const LayerStyleArguments& layerStyle) const
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
