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

std::string PrintableInput::GetFriendlyLayerName() const
{
    return "Input";
}

uint64_t PrintableInput::Size() const
{
    return Input::Size();
}

void PrintableInput::operator=(const layers::Input& input)
{
    Input::operator=(input);
}

void PrintableInput::operator=(const layers::Layer & layer)
{
    operator=(dynamic_cast<const layers::Input&>(layer));
}

LayerLayout PrintableInput::Print(std::ostream& os, double left, double top, uint64_t layerIndex, const PrintArguments& arguments) const
{
    // calculate the layout
    auto layout = PrintableLayer::Print(os, left, top, layerIndex, GetFriendlyLayerName(), Size(), arguments.emptyElementLayout, arguments.layerStyle);

    //// print the visible elements, before the dots
    for (uint64_t k = 0; k < layout.NumVisibleElements() - 1; ++k)
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

layers::Layer::InputCoordinateIterator PrintableInput::GetInputCoordinates(uint64_t index) const
{
    return Input::GetInputCoordinates(index);
}
