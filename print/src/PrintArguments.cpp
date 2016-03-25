////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     PrintArguments.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"

// utilities
#include "Files.h"

void ParsedPrintArguments::AddArgs(utilities::CommandLineParser & parser)
{
    parser.AddOption(inputStackFile, "inputStackFile", "isf", "Path to the input file that contains the stack information", "");
    parser.AddOption(outputSvgFile, "outputSvgFile", "osvg", "Path to the output Svg file", "");

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

    parser.AddOption(stackLayout.horizontalMargin, "layerHorizontalMargin", "lhm", "Horizontal distance to the left edge of each layer", 20);
    parser.AddOption(stackLayout.verticalMargin, "layerVerticalMargin", "lvm", "Vertical distnace to the top edge of the first layer", 10);
    parser.AddOption(stackLayout.verticalSpacing, "layerVerticalSpacing", "lvs", "The amount of vertical space between layers", 30);

    parser.AddOption(layerStyle.maxWidth, "layerMaxWidth", "lmw", "The maximum width of any layer", 700);
    parser.AddOption(layerStyle.cornerRadius, "layerCornerRadius", "lcr", "Radius of layer rounded corners", 10);

    parser.AddOption(edgeStyle.flattness, "edgeFlattness", "ef", "Flatness of edges: between 0 and 1", 0.65);
    parser.AddOption(edgeStyle.dashStyle, "edgeDashStyle", "eds", "The dash style of the edges", "5,2");
}

utilities::CommandLineParseResult ParsedPrintArguments::PostProcess(const utilities::CommandLineParser & parser)
{
    std::vector<std::string> parseErrorMessages;

    if(inputStackFile == "")
    {
        parseErrorMessages.push_back("-inputStackFile (or -isf) is required");
    }
    else
    {
        if(!utilities::IsFileReadable(inputStackFile))
        {
            parseErrorMessages.push_back("cannot read from specified input stack file: " + inputStackFile);
        }
    }

    return parseErrorMessages;
}
