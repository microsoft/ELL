// PrintableMap.cpp

#include "PrintableMap.h"
#include "PrintableInput.h"
#include "PrintableShift.h"
#include "PrintableScale.h"
#include "PrintableSum.h"
#include "svgHelpers.h"

// utilities
#include "StringFormat.h"
using utilities::StringFormat;

// stl
using std::make_shared;

#include <string>
using std::to_string;

#include <memory>
using std::dynamic_pointer_cast;

namespace
{
    const char* styleDefinitionFormat = 
R"aw(
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
        stroke-dasharray:   %s
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
)aw";

    const char* elementDefinitionFormat =
        R"aw(
<defs>
    <g id = "%sElement">
        <ellipse class = "Connector" cx = "%i" cy = "%i" rx = "%i" ry = "%i" />
        <ellipse class = "Connector" cx = "%i" cy = "%i" rx = "%i" ry = "%i" />
        <rect class = "Element" x = "%i" y = "%i" width = "%i" height = "%i" rx = "%i" ry = "%i" />
    </g>
</defs>
)aw";

}

void PrintableMap::Print(ostream & os, const CommandLineArguments& args)
{
    os << "<html>\n<body>\n";

    auto styleDefinition = StringFormat(styleDefinitionFormat, args.edgeStyle.dashStyle);
    os << styleDefinition;

    os << "<svg>\n";

    auto elementDefinition = StringFormat(elementDefinitionFormat, "Value", 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3);
    os << elementDefinition;




//    // print layer by layer
//    double layerTop = args.layerVerticalMargin;
//    double layerLeft = args.layerHorizontalMargin;
//    for (uint64 layerIndex = 0; layerIndex < _layers.size(); ++layerIndex)
//    {
//        auto layer = GetLayer<IPrintableLayer>(layerIndex);
//
//        layer->ComputeLayout(args, layerLeft, layerTop);
//
//        string typeName = layer->GetTypeName();
//        double layerHeight = layer->GetHeight();
//        double layerYMid = layerTop + layerHeight/2.0;
//
//        // draw the layer rectangle
//        svgRect(os, typeName, layerLeft, layerTop, args.layerCornerRadius, layer->GetWidth(), layerHeight);
//        svgText(os, to_string(layerIndex), "Layer", layerLeft + 15, layerYMid);
//        svgText(os, typeName, "Layer", layerLeft + 40, layerYMid, true);
//
//        // let the layer print its contents
//        layer->Print(os);
//
//        // print edges
//        if (layerIndex > 0) // skip input layer
//        {
//            uint64 layerSize = _layers[layerIndex]->Size();
//            for (uint64 column = 0; column<layerSize; ++column)
//            {
//                if (!layer->IsHidden(column)) // if output is hidden, hide edge
//                {
//                    auto inputCoordinates = _layers[layerIndex]->GetInputCoordinates(column);
//                    while (inputCoordinates.IsValid()) // foreach incoming edge
//                    {
//                        auto coordinate = inputCoordinates.Get();
//                        auto inputLayer = GetLayer<IPrintableLayer>(coordinate.GetRow());
//                        if (!inputLayer->IsHidden(coordinate.GetColumn())) // if input is hidden, hide edge
//                        {
//                            svgEdge(os, inputLayer->GetOutputPoint(coordinate.GetColumn()), layer->GetInputPoint(column), args.edgeStyle.flattness);
//                        }
//                        inputCoordinates.Next();
//                    }
//                }
//            }
//        }
//
//        // compute offset of next layer
//        layerTop += layer->GetHeight() + args.layerVerticalSpacing;
//        layerLeft += args.layerHorizontalMarginIncrement;
//    }
//
//    os << 
//R"aw(
//</svg>
//</body>
//</html>
//)aw";
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
        auto upScale = make_shared<PrintableScale>();
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<PrintableShift>();
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

