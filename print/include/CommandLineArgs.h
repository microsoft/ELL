// CommandLineArgs.h

#pragma once

#include "types.h"

#include <string>
using std::string;

struct ElementStyleArgs
{
    double width = 0;
    double height = 0;
    double cornerRadius = 0;
    double connectorRadius = 0;
};

struct EdgeStyleArgs
{
    string dashStyle = "";
    double flattness = 0;
};

struct CommandLineArgs 
{
    double xLayerIndent = 0;
    double yLayerIndent = 0;
    double yLayerSpacing = 0;
    double maxLayerWidth = 0;
    double layerCornerRadius = 0;

    double xElementLeftPadding = 0;
    double xElementRightPadding = 0;
    double yElementPadding = 0;
    double yEmptyElementPadding = 0;
    double xElementSpacing = 0;
    int valueMaxChars = 0;

    double dotsWidth = 0;

    ElementStyleArgs coordinatewiseElementStyle;
    ElementStyleArgs emptyElementStyle;

    EdgeStyleArgs edgeStyle;

    CommandLineArgs()
    {
        xLayerIndent = 20;
        yLayerIndent = 10;
        yLayerSpacing = 20;
        maxLayerWidth = 1200;
        layerCornerRadius = 10;

        xElementLeftPadding = 70;
        xElementRightPadding = 10;
        yElementPadding = 14;
        yEmptyElementPadding = 12;
        xElementSpacing = 5;
        valueMaxChars = 6;

        dotsWidth = 50;

        coordinatewiseElementStyle.width = 55;
        coordinatewiseElementStyle.height = 40;
        coordinatewiseElementStyle.cornerRadius = 5;
        coordinatewiseElementStyle.connectorRadius = 5;

        emptyElementStyle.width = 40;
        emptyElementStyle.height = 20;
        emptyElementStyle.cornerRadius = 5;
        emptyElementStyle.connectorRadius = 5;

        edgeStyle.flattness = 0.7;
        edgeStyle.dashStyle = "4,2";
    }
};