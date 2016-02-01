// PrintableLayer.cpp

#include "PrintableLayer.h"
#include "SvgHelpers.h"

// stl
#include <string>
using std::to_string;

#include <algorithm>
using std::transform;

LayerLayout PrintableLayer::Print(ostream& os, double left, double top, uint64 layerIndex, const string& typeName, uint64 size, const ElementLayoutArguments& elementLayout, const LayerStyleArguments& layerStyle) const
{
    // calculate the layout
    LayerLayout layout(left, top, size, layerStyle.maxWidth, elementLayout);

    // draw the layer rectangle
    SvgRect(os, 2, typeName, left, top, layout.GetWidth(), layout.GetHeight(), layerStyle.cornerRadius);
    
    // print the layer index
    SvgText(os, 2, "Layer", left + 15, layout.GetMidY(), to_string(layerIndex), 0);

    // print the layer type
    string uppercaseTypeName = typeName;
    transform(typeName.cbegin(), typeName.cend(), uppercaseTypeName.begin(), ::toupper);
    SvgText(os, 2, "Layer", left + 40, layout.GetMidY(), uppercaseTypeName, -90);

    return layout;
}
