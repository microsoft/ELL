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

struct ElementStyleArgs
{
    double width;
    double height;
    double cornerRadius;
    double connectorRadius;
    double leftPadding;
    double rightPadding;
    double verticalPadding;
    double horizontalSpacing;
    double dotsWidth;
};

struct EdgeStyleArgs
{
    string dashStyle;
    double flattness;
};

struct CommandLineArguments
{
    string mapFile; // TODO use common args
    string svgFile; // TODO use common args

    double layerHorizontalMargin;
    double layerHorizontalMarginIncrement;
    double layerVerticalMargin;
    double layerVerticalSpacing;
    double layerMaxWidth;
    double layerCornerRadius;

    int valueElementMaxChars;

    ElementStyleArgs valueElementStyle;
    ElementStyleArgs emptyElementStyle;

    EdgeStyleArgs edgeStyle;
};

struct ParsedCommandLineArguments : public CommandLineArguments, public ParsedArgSet
{
    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(mapFile, "inputMapFile", "imf", "Path to the input file that contains the map information", "");
        parser.AddOption(svgFile, "outputSvgFile", "osf", "Path to the output svg file", "");

        // TODO: confirm that the filenames are set. Perhaps add method "AddRequiredOption"

        parser.AddOption(layerHorizontalMargin, "layerHorizontalMargin", "lhm", "Horizontal distance to the left edge of each layer", 20);
        parser.AddOption(layerHorizontalMarginIncrement, "layerHorizontalMarginIncrement", "lhmi", "Ammount by which the horizontal distance to the left edge of each layer increases", 13);
        parser.AddOption(layerVerticalMargin, "layerVerticalMargin", "lvm", "Vertical distnace to the top edge of the first layer", 10);
        parser.AddOption(layerVerticalSpacing, "layerVerticalSpacing", "lvs", "The amount of vertical space between layers", 30);
        parser.AddOption(layerMaxWidth, "layerMaxWidth", "lmw", "The maximum width of any layer", 700);
        parser.AddOption(layerCornerRadius, "layerCornerRadius", "lcr", "Radius of layer rounded corners", 10);
        
        parser.AddOption(valueElementStyle.width, "valueElementWidth", "vew", "Width of each element in a layer that shows values", 55);
        parser.AddOption(valueElementStyle.height, "valueElementHeight", "veh", "Height of each element in a layer that shows values", 40);
        parser.AddOption(valueElementStyle.cornerRadius, "valueElementCornerRadius", "vecr", "Radius of rounded corners of elements in a layer that shows values", 5);
        parser.AddOption(valueElementStyle.connectorRadius, "valueElementConnectorRadius", "vekr", "Radius of connectors on top and bottom of elements in a layer that shows values", 5);
        parser.AddOption(valueElementStyle.leftPadding, "valueElementLeftPadding", "velp", "Horizontal distance between the left edge of a layer that shows values and its first element", 70);
        parser.AddOption(valueElementStyle.rightPadding, "valueElementRightPadding", "verp", "Horizontal distance between the right edge of a layer that shows values and its last element", 10);
        parser.AddOption(valueElementStyle.verticalPadding, "valueElementVerticalPadding", "vevp", "Vertical distance between the top/bottom edges of a layer that shows values and the top/bottom edges of its elements", 14);
        parser.AddOption(valueElementStyle.horizontalSpacing, "valueElementHorizontalSpacing", "vehs", "Horizontal distance between consecutive elements in a layer that shows values", 5);
        parser.AddOption(valueElementStyle.dotsWidth, "valueElementDotsWidth", "vedw", "Minimum width of the gap used to show the three dots, which show up when some of the elements are hidden", 45);
        parser.AddOption(valueElementMaxChars, "valueElementMaxChars", "vemc", "Number of characters used to print the value in each element", 6);

        parser.AddOption(emptyElementStyle.width, "emptyElementWidth", "eew", "Width of each element in a layer that doesn't show values", 40);
        parser.AddOption(emptyElementStyle.height, "emptyElementHeight", "eeh", "Height of each element in a layer that doesn't show values", 20);
        parser.AddOption(emptyElementStyle.cornerRadius, "emptyElementCornerRadius", "eecr", "Radius of rounded corners of elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementStyle.connectorRadius, "emptyElementConnectorRadius", "eekr", "Radius of connectors on top and bottom of elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementStyle.leftPadding, "emptyElementLeftPadding", "eelp", "Horizontal distance between the left edge of a layer that doesn't show values and its first element", 70);
        parser.AddOption(emptyElementStyle.rightPadding, "emptyElementRightPadding", "eerp", "Horizontal distance between the right edge of a layer that doesn't show values and its last element", 10);
        parser.AddOption(emptyElementStyle.verticalPadding, "emptyElementVerticalPadding", "eevp", "Vertical distance between the top/bottom edges of a layer that doesn't show values and the top/bottom edges of its elements", 14);
        parser.AddOption(emptyElementStyle.horizontalSpacing, "emptyElementHorizontalSpacing", "eehs", "Horizontal distance between consecutive elements in a layer that doesn't show values", 5);
        parser.AddOption(emptyElementStyle.dotsWidth, "emptyElementDotsWidth", "eedw", "Minimum width of the gap used to show the three dots, which show up when some of the elements are hidden", 45);

        parser.AddOption(edgeStyle.flattness, "edgeFlattness", "ef", "Flatness of edges: between 0 and 1", 0.65);
        parser.AddOption(edgeStyle.dashStyle, "edgeDashStyle", "eds", "The dash style of the edges", "5,2");
    }
};
