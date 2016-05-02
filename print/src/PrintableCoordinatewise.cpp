////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableCoordinatewise.cpp (print)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableCoordinatewise.h"
#include "SvgHelpers.h"

std::string PrintableCoordinatewise::GetFriendlyLayerName() const
{
    return GetOperationName(GetOperationType());
}

uint64_t PrintableCoordinatewise::Size() const
{
    return Coordinatewise::Size();
}

void PrintableCoordinatewise::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::Coordinatewise&>(layer));
}

void PrintableCoordinatewise::operator=(const layers::Coordinatewise& coordinatewise)
{
    layers::Coordinatewise::operator=(coordinatewise);
}

LayerLayout PrintableCoordinatewise::Print(std::ostream & os, double left, double top, uint64_t layerIndex, const PrintArguments & arguments) const
{
    auto layout = PrintableLayer::PrintLayer(os, left, top, layerIndex, GetFriendlyLayerName(), Size(), arguments.valueElementLayout, arguments.layerStyle);
    PrintableLayer::PrintValueElements(os, layout, _values, arguments.valueElementStyle.maxChars);
    return layout;
}

layers::CoordinateIterator PrintableCoordinatewise::GetInputCoordinateIterator(uint64_t index) const
{
    return Coordinatewise::GetInputCoordinateIterator(index);
}
