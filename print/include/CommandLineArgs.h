// CommandLineArgs.h

#pragma once

#include "types.h"

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
    double xElementSpacing = 0;
    double elementWidth = 0;
    double elementHeight = 0;
    double elementCornerRadius = 0;
    double elementConnectorRadius = 0;
    int valueMaxChars = 0;

    double dotsWidth = 0;

    CommandLineArgs()
    {
        xLayerIndent = 20;
        yLayerIndent = 10;
        yLayerSpacing = 20;
        maxLayerWidth = 800;
        layerCornerRadius = 10;

        xElementLeftPadding = 60;
        xElementRightPadding = 10;
        yElementPadding = 12;
        xElementSpacing = 5;
        elementWidth = 55;
        elementHeight = 40;
        elementCornerRadius = 10;
        elementConnectorRadius = 5;
        valueMaxChars = 6;

        dotsWidth = 50;
    }
};