////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableSum.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableSum.h"
#include "SvgHelpers.h"

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
    auto layout = PrintableLayer::PrintLayer(os, left, top, layerIndex, GetFriendlyLayerName(), GetOutputDimension(), arguments.emptyElementLayout, arguments.layerStyle);

    // print the elements
    PrintableLayer::PrintEmptyElements(os, layout);

    return layout;
}

layers::CoordinateIterator PrintableSum::GetInputCoordinateIterator(uint64_t index) const
{
    return Sum::GetInputCoordinateIterator(index);
}

