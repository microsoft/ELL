// CommandLineArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"
using utilities::ParsedArgSet;
using utilities::CommandLineParser;

// stl
#include <string>
using std::string;

struct ElementLayoutArgs
{
    double width;                       // element width
    double height;                      // element height
    double leftPadding;                 // space between the left edge of the layer and the first element
    double rightPadding;                // space between the right edge of the layer and the last element
    double verticalPadding;             // space above and below each element
    double horizontalSpacing;           // space between adjacent elements
    double dotsWidth;                   // width of the "dots" that represent the hidden elements
    double connectorRadius;             // radius of the input/output connectors
};

struct LayerLayoutArgs
{
    double horizontalMargin;            // space to the left of the first layer
    double verticalMargin;              // space above the first layer
    double verticalSpacing;             // space between consecutive layers
    double maxWidth;                    // maximal layer width
};

struct ElementStyleArgs
{
    double cornerRadius;
    uint64 maxChars;
};

struct EdgeStyleArgs
{
    string dashStyle;
    double flattness;
};

struct LayerStyleArgs
{
    double cornerRadius;
};

struct CommandLineArguments
{
    string mapFile; // TODO use common args
    string svgFile; // TODO use common args

    int valueElementMaxChars;

    ElementLayoutArgs valueElementLayout;
    ElementLayoutArgs emptyElementLayout;
    ElementStyleArgs valueElementStyle;
    ElementStyleArgs emptyElementStyle;
    LayerLayoutArgs layerLayout;
    LayerStyleArgs layerStyle;
    EdgeStyleArgs edgeStyle;
};

struct ParsedCommandLineArguments : public CommandLineArguments, public ParsedArgSet
{
    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(mapFile, "inputMapFile", "imf", "Path to the input file that contains the map information", "");
        parser.AddOption(svgFile, "outputSvgFile", "osf", "Path to the output svg file", "");

        //// TODO: confirm that the filenames are set. Perhaps add method "AddRequiredOption"


        parser.AddOption(valueElementLayout.width, "valueElementWidth", "vew", "Width of each element in a layer that shows values", 55);
        parser.AddOption(valueElementLayout.height, "valueElementHeight", "veh", "Height of each element in a layer that shows values", 40);
        parser.AddOption(valueElementLayout.connectorRadius, "valueElementConnectorRadius", "vekr", "Radius of connectors on top and bottom of elements in a layer that shows values", 5);
        parser.AddOption(valueElementLayout.leftPadding, "valueElementLeftPadding", "velp", "Horizontal distance between the left edge of a layer that shows values and its first element", 70);
        parser.AddOption(valueElementLayout.rightPadding, "valueElementRightPadding", "verp", "Horizontal distance between the right edge of a layer that shows values and its last element", 10);
        parser.AddOption(valueElementLayout.verticalPadding, "valueElementVerticalPadding", "vevp", "Vertical distance between the top/bottom edges of a layer that shows values and the top/bottom edges of its elements", 14);
        parser.AddOption(valueElementLayout.horizontalSpacing, "valueElementHorizontalSpacing", "vehs", "Horizontal distance between consecutive elements in a layer that shows values", 5);
        parser.AddOption(valueElementLayout.dotsWidth, "valueElementDotsWidth", "vedw", "Minimum width of the gap used to show the three dots, which show up when some of the elements are hidden", 45);

        parser.AddOption(valueElementStyle.maxChars, "valueElementMaxChars", "vemc", "Number of characters used to print the value in each element", 6);
        parser.AddOption(valueElementStyle.cornerRadius, "valueElementCornerRadius", "vecr", "Radius of rounded corners of elements in a layer that shows values", 5);

        parser.AddOption(emptyElementLayout.width, "emptyElementWidth", "eew", "Width of each element in a layer that doesn't show values", 40);
        parser.AddOption(emptyElementLayout.height, "emptyElementHeight", "eeh", "Height of each element in a layer that doesn't show values", 20);
        parser.AddOption(emptyElementLayout.connectorRadius, "emptyElementConnectorRadius", "eekr", "Radius of connectors on top and bottom of elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementLayout.leftPadding, "emptyElementLeftPadding", "eelp", "Horizontal distance between the left edge of a layer that doesn't show values and its first element", 70);
        parser.AddOption(emptyElementLayout.rightPadding, "emptyElementRightPadding", "eerp", "Horizontal distance between the right edge of a layer that doesn't show values and its last element", 10);
        parser.AddOption(emptyElementLayout.verticalPadding, "emptyElementVerticalPadding", "eevp", "Vertical distance between the top/bottom edges of a layer that doesn't show values and the top/bottom edges of its elements", 14);
        parser.AddOption(emptyElementLayout.horizontalSpacing, "emptyElementHorizontalSpacing", "eehs", "Horizontal distance between consecutive elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementLayout.dotsWidth, "emptyElementDotsWidth", "eedw", "Minimum width of the gap used to show the three dots, which show up when some of the elements are hidden", 45);

        parser.AddOption(emptyElementStyle.cornerRadius, "emptyElementCornerRadius", "eecr", "Radius of rounded corners of elements in a layer that doesn't show values", 5);

        parser.AddOption(layerLayout.horizontalMargin, "layerHorizontalMargin", "lhm", "Horizontal distance to the left edge of each layer", 20);
        parser.AddOption(layerLayout.verticalMargin, "layerVerticalMargin", "lvm", "Vertical distnace to the top edge of the first layer", 10);
        parser.AddOption(layerLayout.verticalSpacing, "layerVerticalSpacing", "lvs", "The amount of vertical space between layers", 30);
        parser.AddOption(layerLayout.maxWidth, "layerMaxWidth", "lmw", "The maximum width of any layer", 700);

        parser.AddOption(layerStyle.cornerRadius, "layerCornerRadius", "lcr", "Radius of layer rounded corners", 10);

        parser.AddOption(edgeStyle.flattness, "edgeFlattness", "ef", "Flatness of edges: between 0 and 1", 0.65);
        parser.AddOption(edgeStyle.dashStyle, "edgeDashStyle", "eds", "The dash style of the edges", "5,2");
    }
};
