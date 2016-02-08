// PrintArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

struct ElementLayoutArguments
{
    double width;                       // element width
    double height;                      // element height
    double leftPadding;                 // space between the left edge of the layer and the first element
    double rightPadding;                // space between the right edge of the layer and the last element
    double verticalPadding;             // space above and below each element
    double horizontalSpacing;           // space between adjacent elements
    double dotsWidth;                   // width of the "dots" that represent the hidden elements
};

struct MapLayoutArguments
{
    double horizontalMargin;            // space to the left of the first layer
    double verticalMargin;              // space above the first layer
    double verticalSpacing;             // space between consecutive layers
};

struct ElementStyleArguments
{
    double cornerRadius;                // radius of the element corners
    double connectorRadius;             // radius of the input/output connectors
};

struct ValueElementStyleArguments : public ElementStyleArguments
{
    uint64 maxChars;
};

struct EdgeStyleArguments
{
    std::string dashStyle;
    double flattness;
};

struct LayerStyleArguments
{
    double maxWidth;                    // maximal layer width
    double cornerRadius;
};

struct PrintArguments
{
    std::string inputMapFile; // TODO use common Arguments
    std::string outputSvgFile; // TODO use common Arguments

    ElementLayoutArguments valueElementLayout;
    ElementLayoutArguments emptyElementLayout;
    ValueElementStyleArguments valueElementStyle;
    ElementStyleArguments emptyElementStyle;
    MapLayoutArguments mapLayout;
    LayerStyleArguments layerStyle;
    EdgeStyleArguments edgeStyle;
};

struct ParsedPrintArguments : public PrintArguments, public utilities::ParsedArgSet
{
    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(inputMapFile, "inputMapFile", "imf", "Path to the input file that contains the map information", "");
        parser.AddOption(outputSvgFile, "outputSvgFile", "osf", "Path to the output Svg file", "");

        //// TODO: confirm that the filenames are set. Perhaps add method "AddRequiredOption"

        parser.AddOption(valueElementLayout.width, "valueElementWidth", "vew", "Width of each element in a layer that shows values", 55);
        parser.AddOption(valueElementLayout.height, "valueElementHeight", "veh", "Height of each element in a layer that shows values", 40);
        parser.AddOption(valueElementLayout.leftPadding, "valueElementLeftPadding", "velp", "Horizontal distance between the left edge of a layer that shows values and its first element", 70);
        parser.AddOption(valueElementLayout.rightPadding, "valueElementRightPadding", "verp", "Horizontal distance between the right edge of a layer that shows values and its last element", 10);
        parser.AddOption(valueElementLayout.verticalPadding, "valueElementVerticalPadding", "vevp", "Vertical distance between the top/bottom edges of a layer that shows values and the top/bottom edges of its elements", 14);
        parser.AddOption(valueElementLayout.horizontalSpacing, "valueElementHorizontalSpacing", "vehs", "Horizontal distance between consecutive elements in a layer that shows values", 5);
        parser.AddOption(valueElementLayout.dotsWidth, "valueElementDotsWidth", "vedw", "Minimum width of the gap used to show the three dots, which show up when some of the elements are hidden", 45);

        parser.AddOption(valueElementStyle.cornerRadius, "valueElementCornerRadius", "vecr", "Radius of rounded corners of elements in a layer that shows values", 5);
        parser.AddOption(valueElementStyle.connectorRadius, "valueElementConnectorRadius", "vekr", "Radius of connectors on top and bottom of elements in a layer that shows values", 5);
        parser.AddOption(valueElementStyle.maxChars, "valueElementMaxChars", "vemc", "Number of characters used to print the value in each element", 6);

        parser.AddOption(emptyElementLayout.width, "emptyElementWidth", "eew", "Width of each element in a layer that doesn't show values", 40);
        parser.AddOption(emptyElementLayout.height, "emptyElementHeight", "eeh", "Height of each element in a layer that doesn't show values", 28);
        parser.AddOption(emptyElementLayout.leftPadding, "emptyElementLeftPadding", "eelp", "Horizontal distance between the left edge of a layer that doesn't show values and its first element", 70);
        parser.AddOption(emptyElementLayout.rightPadding, "emptyElementRightPadding", "eerp", "Horizontal distance between the right edge of a layer that doesn't show values and its last element", 10);
        parser.AddOption(emptyElementLayout.verticalPadding, "emptyElementVerticalPadding", "eevp", "Vertical distance between the top/bottom edges of a layer that doesn't show values and the top/bottom edges of its elements", 18);
        parser.AddOption(emptyElementLayout.horizontalSpacing, "emptyElementHorizontalSpacing", "eehs", "Horizontal distance between consecutive elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementLayout.dotsWidth, "emptyElementDotsWidth", "eedw", "Minimum width of the gap used to show the three dots, which show up when some of the elements are hidden", 45);

        parser.AddOption(emptyElementStyle.cornerRadius, "emptyElementCornerRadius", "eecr", "Radius of rounded corners of elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementStyle.connectorRadius, "emptyElementConnectorRadius", "eekr", "Radius of connectors on top and bottom of elements in a layer that doesn't show values", 5);

        parser.AddOption(mapLayout.horizontalMargin, "layerHorizontalMargin", "lhm", "Horizontal distance to the left edge of each layer", 20);
        parser.AddOption(mapLayout.verticalMargin, "layerVerticalMargin", "lvm", "Vertical distnace to the top edge of the first layer", 10);
        parser.AddOption(mapLayout.verticalSpacing, "layerVerticalSpacing", "lvs", "The amount of vertical space between layers", 30);

        parser.AddOption(layerStyle.maxWidth, "layerMaxWidth", "lmw", "The maximum width of any layer", 700);
        parser.AddOption(layerStyle.cornerRadius, "layerCornerRadius", "lcr", "Radius of layer rounded corners", 10);

        parser.AddOption(edgeStyle.flattness, "edgeFlattness", "ef", "Flatness of edges: between 0 and 1", 0.65);
        parser.AddOption(edgeStyle.dashStyle, "edgeDashStyle", "eds", "The dash style of the edges", "5,2");
    }
};
