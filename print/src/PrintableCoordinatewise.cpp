// PrintableCoordinatewise.cpp

#include "PrintableCoordinatewise.h"
#include "svgHelpers.h"

PrintableCoordinatewise::PrintableCoordinatewise(const Coordinatewise::DoubleOperation& operation, Layer::Type type) : Coordinatewise(operation, type)
{}

LayerLayout PrintableCoordinatewise::Print(ostream & os, double left, double top, uint64 layerIndex, const CommandLineArguments & args) const
{
    auto layout = PrintableLayer::Print(os, left, top, layerIndex, GetTypeName(), Size(), args.layerLayout.maxWidth, args.valueElementLayout, args.layerStyle);

   //// print the visible elements, before the dots
   for (uint64 k = 0; k < layout.NumVisibleElements()-1; ++k)
   {
        svgValueElement(os, 2, layout.GetMidX(k), layout.GetMidY(), _values[k], args.valueElementStyle.maxChars, k);
   }

   // print last element
   svgValueElement(os, 2, layout.GetMidX(Size() - 1), layout.GetMidY(), _values[Size() - 1], args.valueElementStyle.maxChars, Size() - 1);

   // if has hidden elements, draw the dots
   if(layout.HasHidden())
   {
       svgDots(os, 2, layout.GetDotsMidX(), layout.GetMidY());
   }

   return layout;
}
