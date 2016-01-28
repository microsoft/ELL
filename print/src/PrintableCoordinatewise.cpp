//// PrintableCoordinatewise.cpp
//
//#include "PrintableCoordinatewise.h"
//#include "LayerLayout.h"
//#include "svgHelpers.h"
//
//#include <string>
//using std::to_string;
//
////LayerLayout PrintableCoordinatewise::Print(ostream& os, double left, double top, const CommandLineArguments& args) const
////{
////    LayerLayout layout( uint64 numElements, double layerMaxWidth, ElementLayoutArgs args)
////
////
////
////   double elementTop = _cy - _elementStyle.height/2.0;
////   double elementBottom = elementTop + _elementStyle.height;
////
////   // define the element shape
////   string elementDefName = svgDefineElement(os, (void*)this, _elementStyle);
////
////   // print the visible elements, before the dots
////   for (uint64 k = 0; k < _values.size(); ++k)
////   {
////       if (_upLayout->IsHidden(k))
////       {
////           continue;
////       }
////
////       double elementXMid = _upLayout->GetMidX(k);
////       double elementLeft = elementXMid- _elementStyle.width / 2.0;
////       double value = _values[k];
////       int precision = GetPrecision(value, _valueElementMaxChars);
////
////       svgUse(os, elementDefName, elementLeft, elementTop);
////       svgText(os, value, precision, "Element", elementXMid, _cy - 5);
////       svgText(os, to_string(k), "ElementIndex", elementXMid, _cy + 10);
////
////       auto input = _coordinates[k];
////   }
////
////   // if has hidden elements, draw the dots
////   if(_upLayout->HasHidden())
////   {
////       double dotsXMid = _upLayout->GetDotsMidX();
////       svgDots(os, dotsXMid, _cy);
////   }
////}
////
////void PrintableCoordinatewise::ComputeLayout(const CommandLineArguments& args, double layerLeft, double layerTop)
////{
////    _upLayout = make_unique<LayerLayout>(_values.size(), layerLeft, args.layerMaxWidth, args.valueElementStyle.width, args.valueElementStyle.horizontalSpacing, args.valueElementStyle.leftPadding, args.valueElementStyle.rightPadding, args.valueElementStyle.dotsWidth); // just pass the entire tyle struct
////
////    _layerHeight = args.valueElementStyle.height + 2*args.valueElementStyle.verticalPadding;
////    _cy = layerTop + _layerHeight / 2.0;   
////    _elementStyle = args.valueElementStyle;
////    _valueElementMaxChars = args.valueElementMaxChars;
////}
////
