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
using std::endl;

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
    
    rect.Scale
    {
        fill:        #06aed5;
    }

    rect.Shift
    {
        fill:        #f15156;
    }

    rect.Sum
    {
        fill:        #cf4eff;
    }

    rect.Input
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
    <g id="%sElement">
        <ellipse class="Connector" cx="%i" cy="%i" rx="%i" ry="%i" />
        <ellipse class="Connector" cx="%i" cy="%i" rx="%i" ry="%i" />
        <rect class="Element" x="0" y="0" width="%i" height="%i" rx="%i" ry="%i" />
    </g>
</defs>
)aw";

}

void PrintableMap::Print(ostream & os, const CommandLineArguments& args)
{
    os << "<html>\n<body>\n";
    os << StringFormat(styleDefinitionFormat, args.edgeStyle.dashStyle);

    os << "<svg>\n";

    // define element shapes
    os << StringFormat(elementDefinitionFormat,
        "Value",
        args.valueElementLayout.width / 2.0, 0, args.valueElementLayout.connectorRadius, args.valueElementLayout.connectorRadius,
        args.valueElementLayout.width / 2.0, args.valueElementLayout.height, args.valueElementLayout.connectorRadius, args.valueElementLayout.connectorRadius,
        args.valueElementLayout.width, args.valueElementLayout.height, args.valueElementStyle.cornerRadius, args.valueElementStyle.cornerRadius);

    os << endl;
    os << StringFormat(elementDefinitionFormat,
        "Empty",
        args.emptyElementLayout.width / 2.0, 0, args.emptyElementLayout.connectorRadius, args.emptyElementLayout.connectorRadius,
        args.emptyElementLayout.width / 2.0, args.emptyElementLayout.height, args.emptyElementLayout.connectorRadius, args.emptyElementLayout.connectorRadius,
        args.emptyElementLayout.width, args.emptyElementLayout.height, args.emptyElementStyle.cornerRadius, args.emptyElementStyle.cornerRadius);

    // print layer by layer
    double layerTop = args.layerLayout.verticalMargin;
    vector<LayerLayout> layouts;

    for (uint64 layerIndex = 0; layerIndex < _layers.size(); ++layerIndex)
    {
        auto printableLayer = GetLayer<PrintableLayer>(layerIndex);

        auto layout = printableLayer->Print(os, args.layerLayout.horizontalMargin, layerTop, args); // TODO args not needed

        layerTop += layout.GetHeight() + args.layerLayout.verticalSpacing;


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
//                        auto inputLayer = GetLayer<PrintableLayer>(coordinate.GetRow());
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

