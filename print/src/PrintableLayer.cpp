// PrintableLayer.cpp

#include "PrintableLayer.h"
#include "svgHelpers.h"

// stl
#include <string>
using std::to_string;

#include <algorithm>
using std::transform;

void PrintableLayer::Print(ostream& os, double left, double top, uint64 layerIndex, const string& typeName, const LayerLayout& layout, const LayerStyleArgs& args) const
{
    // draw the layer rectangle
    svgRect(os, 2, typeName, left, top, layout.GetWidth(), layout.GetHeight(), args.cornerRadius);
    
    // print the layer index
    svgText(os, 2, "Layer", left + 15, top+layout.GetMidY(), to_string(layerIndex), 0);

    // print the layer type
    string uppercaseTypeName = typeName;
    transform(typeName.cbegin(), typeName.cend(), uppercaseTypeName.begin(), ::toupper);
    svgText(os, 2, "Layer", left + 40, top+layout.GetMidY(), uppercaseTypeName, -90);
}
