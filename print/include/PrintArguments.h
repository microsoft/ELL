////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintArguments.h (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

/// <summary> Arguments for element layout. </summary>
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

/// <summary> Arguments for map layout. </summary>
struct MapLayoutArguments
{
    double horizontalMargin;            // space to the left of the first layer
    double verticalMargin;              // space above the first layer
    double verticalSpacing;             // space between consecutive layers
};

/// <summary> Arguments for element style. </summary>
struct ElementStyleArguments
{
    double cornerRadius;                // radius of the element corners
    double connectorRadius;             // radius of the input/output connectors
};

/// <summary> Arguments for value element style. </summary>
struct ValueElementStyleArguments : public ElementStyleArguments
{
    uint64 maxChars;
};

/// <summary> Arguments for edge style. </summary>
struct EdgeStyleArguments
{
    std::string dashStyle;
    double flattness;
};

/// <summary> Arguments for layer style. </summary>
struct LayerStyleArguments
{
    double maxWidth;                    // maximal layer width
    double cornerRadius;
};

/// <summary> Arguments for print. </summary>
struct PrintArguments
{
    std::string inputMapFile;
    std::string outputSvgFile;

    ElementLayoutArguments valueElementLayout;
    ElementLayoutArguments emptyElementLayout;
    ValueElementStyleArguments valueElementStyle;
    ElementStyleArguments emptyElementStyle;
    MapLayoutArguments mapLayout;
    LayerStyleArguments layerStyle;
    EdgeStyleArguments edgeStyle;
};

/// <summary> Arguments for parsed print. </summary>
struct ParsedPrintArguments : public PrintArguments, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments. </summary>
    ///
    /// <param name="parser"> [in,out] The parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);

    /// <summary> Check arguments. </summary>
    ///
    /// <param name="parser"> The parser. </param>
    ///
    /// <returns> An utilities::ParseResult. </returns>
    virtual utilities::ParseResult PostProcess(const utilities::CommandLineParser& parser);

};
