////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
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

LayerLayout PrintableCoordinatewise::Print(std::ostream & os, double left, double top, uint64_t layerIndex, const PrintArguments & Arguments) const
{
    auto layout = PrintableLayer::Print(os, left, top, layerIndex, GetFriendlyLayerName(), Size(), Arguments.valueElementLayout, Arguments.layerStyle);

   //// print the visible elements, before the dots
   for (uint64_t k = 0; k < layout.NumVisibleElements()-1; ++k)
   {
        SvgValueElement(os, 2, layout.GetMidX(k), layout.GetMidY(), _values[k], Arguments.valueElementStyle.maxChars, k);
   }

   // print last element
   SvgValueElement(os, 2, layout.GetMidX(Size() - 1), layout.GetMidY(), _values[Size() - 1], Arguments.valueElementStyle.maxChars, Size() - 1);

   // if has hidden elements, draw the dots
   if(layout.HasHidden())
   {
       SvgDots(os, 2, layout.GetDotsMidX(), layout.GetMidY());
   }

   return layout;
}

layers::Layer::InputCoordinateIterator PrintableCoordinatewise::GetInputCoordinates(uint64_t index) const
{
    return Coordinatewise::GetInputCoordinates(index);
}
