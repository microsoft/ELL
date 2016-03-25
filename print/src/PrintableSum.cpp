////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     PrintableSum.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableSum.h"
#include "SvgHelpers.h"

std::string PrintableSum::GetFriendlyLayerName() const
{
    return "Sum";
}

uint64_t PrintableSum::Size() const
{
    return Sum::Size();
}

void PrintableSum::operator=(const layers::Sum& sum)
{
    Sum::operator=(sum);
}

void PrintableSum::operator=(const layers::Layer & layer)
{
    operator=(dynamic_cast<const layers::Sum&>(layer));
}

LayerLayout PrintableSum::Print(std::ostream& os, double left, double top, uint64_t layerIndex, const PrintArguments& arguments) const
{
    // calculate the layout
    auto layout = PrintableLayer::Print(os, left, top, layerIndex, GetFriendlyLayerName(), Size(), arguments.emptyElementLayout, arguments.layerStyle);

    //// print the visible elements, before the dots
    for (uint64_t k = 0; k < layout.NumVisibleElements() - 1; ++k)
    {
        SvgEmptyElement(os, 2, layout.GetMidX(k), layout.GetMidY(), k);
    }

    // print last element
    SvgEmptyElement(os, 2, layout.GetMidX(Size() - 1), layout.GetMidY(), Size() - 1);

    // if has hidden elements, draw the dots
    if (layout.HasHidden())
    {
        SvgDots(os, 2, layout.GetDotsMidX(), layout.GetMidY());
    }

    return layout;
}

layers::Layer::InputCoordinateIterator PrintableSum::GetInputCoordinates(uint64_t index) const
{
    return Sum::GetInputCoordinates(index);
}

