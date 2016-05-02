////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableModel.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableModel.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"
#include "PrintableLayer.h"
#include "SvgHelpers.h"

// utilities
#include "Format.h"

// stl
#include <string>
#include <memory>
#include <functional>
#include <vector>

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
            stroke:         #222222;
            stroke-width:   2;
        }
    
        rect.%
        {
            fill:           #06aed5;
        }

        rect.%
        {
            fill:           #f15156;
        }

        rect.Sum
        {
            fill:           #cf4eff;
        }

        rect.Input
        {
            fill:           #bbbbbb;
        }

        rect.Element
        {
            fill:           white;
        }

        ellipse.Connector
        {
            fill:           #222222;
        }

        path.Edge
        {
            stroke:             #110011;
            stroke-width:       2;
            fill:               none;
            stroke-dasharray:   %;
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
}

void PrintElementDefinition(std::ostream& os, const std::string& id, double width, double height, double connectorRadius, double cornerRadius, bool hasInputConnector=true)
{
    os << "            <g id=\"" << id << "\">\n";
    if (hasInputConnector)
    {
        SvgCircle(os, 4, "Connector", 0, -height / 2, connectorRadius);
    }
    SvgCircle(os, 4, "Connector", 0, height/2, connectorRadius);
    SvgRect(os, 4, "Element", -width/2, -height/2, width, height, cornerRadius);
    os << "            </g>\n";
}

PrintableModel::PrintableModel(const layers::Model& other) : _inputLayerSize(other.GetRequiredLayerSize(0))
{
    utilities::TypeFactory<PrintableLayer> printableLayerFactory;
    printableLayerFactory.AddType<PrintableCoordinatewise>(layers::Coordinatewise::GetTypeName());
    printableLayerFactory.AddType<PrintableSum>(layers::Sum::GetTypeName());

    // input layer is built-in, start from layer 1
    for (uint64_t index = 1; index < other.NumLayers(); ++index)
    {
        const auto& layer = other.GetLayer(index);
        _printableLayers.push_back(printableLayerFactory.Construct(layer.GetRuntimeTypeName())); 
        (*_printableLayers.back()) = layer;
    }
}

void PrintableModel::Print(std::ostream & os, const PrintArguments& arguments)
{
    os << "<html>\n<body>\n";
    utilities::PrintFormat(os, 
        styleDefinitionFormat, 
        layers::Coordinatewise::GetOperationName(layers::Coordinatewise::OperationType::add),
        layers::Coordinatewise::GetOperationName(layers::Coordinatewise::OperationType::multiply),
        arguments.edgeStyle.dashStyle);

    os << "    <svg>\n\n        <defs>\n";
    PrintElementDefinition(os,
        "ValueElement",
        arguments.valueElementLayout.width, 
        arguments.valueElementLayout.height, 
        arguments.valueElementStyle.connectorRadius, 
        arguments.valueElementStyle.cornerRadius);
    
    PrintElementDefinition(os,
        "EmptyElement",
        arguments.emptyElementLayout.width,
        arguments.emptyElementLayout.height,
        arguments.emptyElementStyle.connectorRadius,
        arguments.emptyElementStyle.cornerRadius);

    PrintElementDefinition(os,
        "InputElement",
        arguments.emptyElementLayout.width,
        arguments.emptyElementLayout.height,
        arguments.emptyElementStyle.connectorRadius,
        arguments.emptyElementStyle.cornerRadius,
        false);

    os << "        </defs>\n\n";

    // print
    double layerTop = arguments.modelLayout.verticalMargin;
    std::vector<LayerLayout> layouts;

    // print input layer
    {
        auto inputLayout = PrintableLayer::PrintLayer(os, arguments.modelLayout.horizontalMargin, layerTop, 0, "Input", _inputLayerSize, arguments.emptyElementLayout, arguments.layerStyle);
        PrintableLayer::PrintEmptyElements(os, inputLayout);
        layerTop += inputLayout.GetHeight() + arguments.modelLayout.verticalSpacing;
        layouts.push_back(std::move(inputLayout));
        os << std::endl;
    }

    // print layer by layer
    for (uint64_t i = 0; i < _printableLayers.size(); ++i)
    {
        uint64_t layerIndex = i + 1;
        const auto& printableLayer = _printableLayers[i];
        auto layout = printableLayer->Print(os, arguments.modelLayout.horizontalMargin, layerTop, layerIndex, arguments);
        layerTop += layout.GetHeight() + arguments.modelLayout.verticalSpacing;
        os << std::endl;

        // print edges
        uint64_t layerSize = printableLayer->Size();
        for (uint64_t elementIndex = 0; elementIndex<layerSize; ++elementIndex)
        {
            if (!layout.IsHidden(elementIndex)) // if output is hidden, hide edge
            {
                auto inputCoordinates = printableLayer->GetInputCoordinateIterator(elementIndex);
                while (inputCoordinates.IsValid()) // foreach incoming edge
                {
                    auto coordinate = inputCoordinates.Get();
                    const auto& inputLayout = layouts[coordinate.GetLayerIndex()];
                    if (!inputLayout.IsHidden(coordinate.GetElementIndex())) // if input is hidden, hide edge
                    {
                        SvgEdge(os, 2, inputLayout.GetOutputPoint(coordinate.GetElementIndex()), layout.GetInputPoint(elementIndex), arguments.edgeStyle.flattness);
                    }

                    // on to the next input
                    inputCoordinates.Next();
                }
            }
        }

        // add the current layer's layout to the list of layouts
        layouts.push_back(std::move(layout));

        os << std::endl;
    }

    os << "\n    </svg>\n\n<html>\n<body>\n";
}

