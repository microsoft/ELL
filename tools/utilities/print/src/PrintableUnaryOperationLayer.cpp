////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableUnaryOperationLayer.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableUnaryOperationLayer.h"
#include "SvgHelpers.h"

std::string PrintableUnaryOperationLayer::GetFriendlyLayerName() const
{
    return GetOperationName(GetOperationType());
}

uint64_t PrintableUnaryOperationLayer::GetOutputDimension() const
{
    return UnaryOperationLayer::GetOutputDimension();
}

void PrintableUnaryOperationLayer::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::UnaryOperationLayer&>(layer));
}

void PrintableUnaryOperationLayer::operator=(const layers::UnaryOperationLayer& layer)
{
    layers::UnaryOperationLayer::operator=(layer);
}

LayerLayout PrintableUnaryOperationLayer::Print(std::ostream & os, double left, double top, uint64_t layerIndex, const PrintArguments & arguments) const
{
    auto layout = PrintableLayer::PrintLayer(os, left, top, layerIndex, GetFriendlyLayerName(), GetOutputDimension(), arguments.valueElementLayout, arguments.layerStyle);
    return layout;
}

layers::CoordinateIterator PrintableUnaryOperationLayer::GetInputCoordinateIterator(uint64_t index) const
{
    return UnaryOperationLayer::GetInputCoordinateIterator(index);
}
