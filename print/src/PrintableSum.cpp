// PrintableSum.cpp

#include "PrintableSum.h"

LayerLayout PrintableSum::Print(ostream& os, double left, double top, uint64 layerIndex, const CommandLineArguments& args) const
{
    // calculate the layout
    LayerLayout layout(Size(), args.layerLayout.maxWidth, args.emptyElementLayout);

    PrintableLayer::Print(os, left, top, layerIndex, "Sum", layout, args.layerStyle);

    return layout;
}
