// PrintableCoordinatewise.cpp

#include "PrintableCoordinatewise.h"
#include "svgHelpers.h"

PrintableCoordinatewise::PrintableCoordinatewise(const Coordinatewise::DoubleOperation& operation, Layer::Type type) : Coordinatewise(operation, type)
{}

LayerLayout PrintableCoordinatewise::Print(ostream & os, double left, double top, uint64 layerIndex, const CommandLineArguments & args) const
{
    // calculate the layout
    LayerLayout layout(Size(), args.layerLayout.maxWidth, args.valueElementLayout);

    PrintableLayer::Print(os, left, top, layerIndex, GetTypeName(), layout, args.layerStyle);

   //// print the visible elements, before the dots
   for (uint64 k = 0; k < layout.NumVisibleElements()-1; ++k)
   {
        svgValueElement(os, 2, left + layout.GetMidX(k), top + layout.GetMidY(), _values[k], args.valueElementMaxChars, k);
   }

   // print last element
   svgValueElement(os, 2, left + layout.GetMidX(Size() - 1), top + layout.GetMidY(), _values[Size() - 1], args.valueElementMaxChars, Size() - 1);

   // if has hidden elements, draw the dots
   if(layout.HasHidden())
   {
       svgDots(os, 2, left+layout.GetDotsMidX(), top+layout.GetMidY());
   }

   return layout;
}
