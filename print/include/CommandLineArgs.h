// CommandLineArgs.h

#pragma once

struct CommandLineArgs
{
    double xLayerIndent = 0;
    double yLayerIndent = 0;
    double yLayerSpacing = 0;
    double maxLayerWidth = 0;

    double xElementLeftPadding = 0;
    double xElementRightPadding = 0;
    double yElementPadding = 0;
    double xElementSpacing = 0;
    double elementWidth = 0;
    double elementHeight = 0;
    double dotsWidth = 0;

    CommandLineArgs()
    {
        xLayerIndent = 20;
        yLayerIndent = 10;
        yLayerSpacing = 20;
        maxLayerWidth = 800;

        xElementLeftPadding = 60;
        xElementRightPadding = 10;
        yElementPadding = 10;
        xElementSpacing = 5;
        elementWidth = 50;
        elementHeight = 40;
        dotsWidth = 50;
    }
};