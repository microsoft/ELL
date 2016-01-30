// PrintableLayer.cpp

#include "PrintableLayer.h"
#include "svgHelpers.h"

// stl
#include <string>
using std::to_string;

void PrintableLayer::Print(ostream& os, double left, double top, uint64 layerIndex, const string& typeName, const LayerLayout& layout, const LayerStyleArgs& args) const
{
    // draw the layer rectangle
    svgRect(os, 2, typeName, left, top, layout.GetWidth(), layout.GetHeight(), args.cornerRadius);
    svgText(os, 2, "Layer", left + 15, top+layout.GetMidY(), to_string(layerIndex), 0);
    svgText(os, 2, "Layer", left + 40, top+layout.GetMidY(), typeName, -90);
}
