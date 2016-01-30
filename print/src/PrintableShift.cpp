// PrintableShift.cpp

#include "PrintableShift.h"
#include "svgHelpers.h"

LayerLayout PrintableShift::Print(ostream & os, double left, double top, uint64 layerIndex, const CommandLineArguments & args) const
{
    // calculate the layout
    LayerLayout layout(Size(), args.layerLayout.maxWidth, args.valueElementLayout);

    PrintableLayer::Print(os, left, top, layerIndex, GetTypeName(), layout, args.layerStyle);

   //// define the element shape
   //string elementDefName = svgDefineElement(os, (void*)this, args.valueElementStyle);

   //// print the visible elements, before the dots
   //for (uint64 k = 0; k < _values.size(); ++k)
   //{
   //    if (_upLayout->IsHidden(k))
   //    {
   //        continue;
   //    }

   //    double elementXMid = _upLayout->GetMidX(k);
   //    double elementLeft = elementXMid- _elementStyle.width / 2.0;
   //    double value = _values[k];
   //    int precision = GetPrecision(value, _valueElementMaxChars);

   //    svgUse(os, elementDefName, elementLeft, elementTop);
   //    svgText(os, value, precision, "Element", elementXMid, _cy - 5);
   //    svgText(os, to_string(k), "ElementIndex", elementXMid, _cy + 10);

   //    auto input = _coordinates[k];
   //}

   //// if has hidden elements, draw the dots
   //if(_upLayout->HasHidden())
   //{
   //    double dotsXMid = _upLayout->GetDotsMidX();
   //    svgDots(os, dotsXMid, _cy);
   //}

   return layout;
}

//void PrintableCoordinatewise::ComputeLayout(const CommandLineArguments& args, double layerLeft, double layerTop)
//{
    //_upLayout = make_unique<LayerLayout>(_values.size(), layerLeft, args.layerMaxWidth, args.valueElementStyle.width, args.valueElementStyle.horizontalSpacing, args.valueElementStyle.leftPadding, args.valueElementStyle.rightPadding, args.valueElementStyle.dotsWidth); // just pass the entire tyle struct

    //_layerHeight = args.valueElementStyle.height + 2*args.valueElementStyle.verticalPadding;
    //_cy = layerTop + _layerHeight / 2.0;   
    //_elementStyle = args.valueElementStyle;
    //_valueElementMaxChars = args.valueElementMaxChars;
//}

