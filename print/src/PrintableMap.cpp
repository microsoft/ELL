// PrintableMap.cpp

#include "PrintableMap.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"
#include "svgHelpers.h"

using std::make_shared;

#include <string>
using std::to_string;

void PrintableMap::Print(ostream & os, const CommandLineArgs& args)
{

    os << 
R"aw(
<html>
<body>

<style>
    text
    {
        font-family:    sans-serif;
    }

    rect
    {
        stroke:        #222222;
        stroke-width:    2;
    }
    
    rect.InputLayer
    {
        fill:        #22AA22;
    }

    rect.SCALE
    {
        fill:        #FF2244;
    }

    rect.SHIFT
    {
        fill:        #22FF44;
    }

    rect.SUM
    {
        fill:        #2244FF;
    }

    rect.IN
    {
        fill:        #AA0044;
    }

    rect.Element
    {
        fill:        white;
    }

    ellipse.Connector
    {
        fill:        #222222;
    }

    path.Edge
    {
        stroke:             #110011;
        stroke-width:       2;
        fill:               none;
        stroke-dasharray:   4,2;
    }

    text.Layer
    {
        fill:           white;
        font:           bold 15 sans-serif;        
    }

    text.Element
    {
        fill:           black;
        font:           15 sans-serif;        
    }

    text.ElementIndex
    {
        fill:           #666666;
        font:           9 sans-serif;        
    }

</style>

<svg>
)aw";

    // print layer by layer
    double yOffset = args.yLayerIndent;
    for (uint64 k = 0; k < _printables.size(); ++k)
    {
        _printables[k]->ComputeLayout(args, yOffset);

        string typeName = _printables[k]->GetTypeName();
        double layerLeft = args.xLayerIndent;
        double layerHeight = _printables[k]->GetHeight();
        double layerYMid = yOffset + layerHeight/2.0;

        // draw the layer rectangle
        svgRect(os, typeName, layerLeft, yOffset, args.layerCornerRadius, _printables[k]->GetWidth(), layerHeight);
        svgText(os, to_string(k), "Layer", layerLeft + 15, layerYMid);
        svgText(os, typeName, "Layer", layerLeft + 40, layerYMid, true);

        // let the layer print its contents
        _printables[k]->Print(os, k, _printables);
        yOffset += _printables[k]->GetHeight() + args.yLayerSpacing;
    }

    os << 
R"aw(
</svg>
</body>
</html>
)aw";
}

void PrintableMap::Deserialize(JsonSerializer & serializer)
{
    serializer.Read("layers", _printables, PrintableMap::DeserializeLayers);
}

void PrintableMap::DeserializeLayers(JsonSerializer & serializer, shared_ptr<IPrintable>& up)
{
    auto type = serializer.Read<string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Zero")
    {
        auto upZero = make_shared<PrintableInput>();
        upZero->Deserialize(serializer, version);
        up = upZero;
    }
    else if (type == "Scale")
    {
        auto upScale = make_shared<PrintableCoordinatewise>("SCALE");
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<PrintableCoordinatewise>("SHIFT");
        upShift->Deserialize(serializer, version);
        up = upShift;
    }
    else if (type == "Sum")
    {
        auto upSum = make_shared<PrintableSum>();
        upSum->Deserialize(serializer, version);
        up = upSum;
    }
    else
    {
        throw runtime_error("unidentified type in map file: " + type);
    }
}

