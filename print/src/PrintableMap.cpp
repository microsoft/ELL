// PrintableMap.cpp

#include "PrintableMap.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"

using std::make_shared;

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

    rect.ScaleLayer
    {
        fill:        #FF2244;
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

    double yOffset = args.yLayerIndent;

    for (uint64 k = 0; k < _printables.size(); ++k)
    {
        _printables[k]->ComputeLayout(args, yOffset);
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
        auto upScale = make_shared<PrintableCoordinatewise>("SCALE", "ScaleLayer");
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<PrintableCoordinatewise>("SHIFT", "ShiftLayer");
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

