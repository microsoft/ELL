// PrintableMap.cpp

#include "PrintableMap.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"
#include "svgHelpers.h"

using std::make_shared;

#include <string>
using std::to_string;

#include <memory>
using std::dynamic_pointer_cast;

void PrintableMap::Print(ostream & os, const CommandLineArguments& args)
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
    
    rect.SCALE
    {
        fill:        #06aed5;
    }

    rect.SHIFT
    {
        fill:        #f15156;
    }

    rect.SUM
    {
        fill:        #cf4eff;
    }

    rect.IN
    {
        fill:        #bbbbbb;
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
        stroke-dasharray:   )aw"; 
    
    os << args.edgeStyle.dashStyle;
        
    os <<
R"aw(;
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
    double layerTop = args.layerVerticalMargin;
    double layerLeft = args.layerHorizontalMargin;
    for (uint64 layerIndex = 0; layerIndex < _layers.size(); ++layerIndex)
    {
        auto printableLayer = GetLayer<IPrintableLayer>(layerIndex);

        printableLayer->ComputeLayout(args, layerLeft, layerTop);

        string typeName = printableLayer->GetTypeName();
        double layerHeight = printableLayer->GetHeight();
        double layerYMid = layerTop + layerHeight/2.0;

        // draw the layer rectangle
        svgRect(os, typeName, layerLeft, layerTop, args.layerCornerRadius, printableLayer->GetWidth(), layerHeight);
        svgText(os, to_string(layerIndex), "Layer", layerLeft + 15, layerYMid);
        svgText(os, typeName, "Layer", layerLeft + 40, layerYMid, true);

        // let the layer print its contents
        printableLayer->Print(os);

        // print edges
        if (layerIndex > 0) // skip input layer
        {
            uint64 layerSize = _layers[layerIndex]->Size();
            for (uint64 column = 0; column<layerSize; ++column)
            {
                if (!printableLayer->IsHidden(column)) // if output is hidden, hide edge
                {
                    auto inputCoordinates = _layers[row]->GetInputCoordinates(column);
                    while (inputCoordinates.IsValid()) // foreach incoming edge
                    {
                        auto inputCoord = inputCoordinates.Get();
                        if (!_layers[inputCoord.GetRow()]->IsHidden(inputCoord.GetColumn())) // if input is hidden, hide edge
                        {
                            svgEdge(os, _layers[inputCoord.GetRow()]->GetBeginPoint(inputCoord.GetColumn()), _layers[row]->GetEndPoint(column), args.edgeStyle.flattness);
                        }
                        inputCoordinates.Next();
                    }
                }
            }
        }

        // compute offset of next layer
        layerTop += _layers[row]->GetHeight() + args.layerVerticalSpacing;
        layerLeft += args.layerHorizontalMarginIncrement;
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
    serializer.Read("layers", _layers, PrintableMap::DeserializeLayers);
}

void PrintableMap::DeserializeLayers(JsonSerializer & serializer, shared_ptr<Layer>& up)
{
    auto type = serializer.Read<string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Input")
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

