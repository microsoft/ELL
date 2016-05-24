////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableCoordinatewise.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableBinaryOperationLayer.h"
#include "SvgHelpers.h"

std::string PrintableBinaryOperationLayer::GetFriendlyLayerName() const
{
    return GetOperationName(GetOperationType());
}

uint64_t PrintableBinaryOperationLayer::GetOutputDimension() const
{
    return layers::BinaryOperationLayer::GetOutputDimension();
}

void PrintableBinaryOperationLayer::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::BinaryOperationLayer&>(layer));
}

void PrintableBinaryOperationLayer::operator=(const layers::BinaryOperationLayer& layer)
{
    layers::BinaryOperationLayer::operator=(layer);
}

LayerLayout PrintableBinaryOperationLayer::Print(std::ostream & os, double left, double top, uint64_t layerIndex, const PrintArguments & arguments) const
{
    auto layout = PrintableLayer::PrintLayer(os, left, top, layerIndex, GetFriendlyLayerName(), GetOutputDimension(), arguments.valueElementLayout, arguments.layerStyle);
    //PrintableLayer::PrintValueElements(os, layout, _values, arguments.valueElementStyle.maxChars);
    return layout;
}

layers::CoordinateIterator PrintableBinaryOperationLayer::GetInputCoordinateIterator(uint64_t index) const
{
    return BinaryOperationLayer::GetInputCoordinateIterator(index);
}
