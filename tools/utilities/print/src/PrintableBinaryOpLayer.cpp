////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PrintableCoordinatewise.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableBinaryOpLayer.h"
#include "SvgHelpers.h"

std::string PrintableBinaryOpLayer::GetFriendlyLayerName() const
{
    return GetOperationName(GetOperationType());
}

uint64_t PrintableBinaryOpLayer::GetOutputDimension() const
{
    return layers::BinaryOpLayer::GetOutputDimension();
}

void PrintableBinaryOpLayer::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::BinaryOpLayer&>(layer));
}

void PrintableBinaryOpLayer::operator=(const layers::BinaryOpLayer& layer)
{
    layers::BinaryOpLayer::operator=(layer);
}

LayerLayout PrintableBinaryOpLayer::Print(std::ostream & os, double left, double top, uint64_t layerIndex, const PrintArguments & arguments) const
{
    auto layout = PrintableLayer::PrintLayer(os, left, top, layerIndex, GetFriendlyLayerName(), GetOutputDimension(), arguments.valueElementLayout, arguments.layerStyle);
    //PrintableLayer::PrintValueElements(os, layout, _values, arguments.valueElementStyle.maxChars);
    return layout;
}

layers::CoordinateIterator PrintableBinaryOpLayer::GetInputCoordinateIterator(uint64_t index) const
{
    return BinaryOpLayer::GetInputCoordinateIterator(index);
}
