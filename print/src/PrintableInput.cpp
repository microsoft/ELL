////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     PrintableInput.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableInput.h"
#include "SvgHelpers.h"

LayerLayout PrintableInput::Print(ostream& os, double left, double top, uint64 layerIndex, const PrintArguments& Arguments) const
{
    // calculate the layout
    auto layout = PrintableLayer::Print(os, left, top, layerIndex, GetTypeName(), Size(), Arguments.emptyElementLayout, Arguments.layerStyle);

    //// print the visible elements, before the dots
    for (uint64 k = 0; k < layout.NumVisibleElements() - 1; ++k)
    {
        SvgInputElement(os, 2, layout.GetMidX(k), layout.GetMidY(), k);
    }

    // print last element
    SvgInputElement(os, 2, layout.GetMidX(Size() - 1), layout.GetMidY(), Size() - 1);

    // if has hidden elements, draw the dots
    if (layout.HasHidden())
    {
        SvgDots(os, 2, layout.GetDotsMidX(), layout.GetMidY());
    }

    return layout;
}
