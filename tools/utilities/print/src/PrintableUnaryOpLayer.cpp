////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableUnaryOpLayer.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableUnaryOpLayer.h"
#include "SvgHelpers.h"

std::string PrintableUnaryOpLayer::GetFriendlyLayerName() const
{
    return GetOperationName(GetOperationType());
}

uint64_t PrintableUnaryOpLayer::GetOutputDimension() const
{
    return UnaryOpLayer::GetOutputDimension();
}

void PrintableUnaryOpLayer::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::UnaryOpLayer&>(layer));
}

void PrintableUnaryOpLayer::operator=(const layers::UnaryOpLayer& layer)
{
    layers::UnaryOpLayer::operator=(layer);
}

LayerLayout PrintableUnaryOpLayer::Print(std::ostream & os, double left, double top, uint64_t layerIndex, const PrintArguments & arguments) const
{
    auto layout = PrintableLayer::PrintLayer(os, left, top, layerIndex, GetFriendlyLayerName(), GetOutputDimension(), arguments.valueElementLayout, arguments.layerStyle);
    // PrintableLayer::PrintValueElements(os, layout, _values, arguments.valueElementStyle.maxChars);
    return layout;
}

layers::CoordinateIterator PrintableUnaryOpLayer::GetInputCoordinateIterator(uint64_t index) const
{
    return UnaryOpLayer::GetInputCoordinateIterator(index);
}
