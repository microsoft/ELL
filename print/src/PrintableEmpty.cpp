// PrintableEmpty.cpp

#include "PrintableEmpty.h"
#include "svgHelpers.h"

using std::make_unique;

#include <string>
using std::to_string;

//void PrintableEmpty::Print(ostream & os) const
//{
//    double halfWidth = _elementStyle.width / 2.0;
//    double elementTop = _cy - _elementStyle.height / 2.0;
//    double elementBottom = elementTop + _elementStyle.height;
//
//    // define the element shape
//    string elementDefName = svgDefineElement(os, (void*)this, _elementStyle);
//
//    // print the visible elements, before the dots
//    for (uint64 k = 0; k < Size(); ++k)
//    {
//        if (!_upLayout->IsHidden(k))
//        {
//            double elementXMid = _upLayout->GetMidX(k);
//            double elementLeft = elementXMid - halfWidth;
//            svgUse(os, elementDefName, elementLeft, elementTop);
//            svgText(os, to_string(k), "ElementIndex", elementXMid, _cy);
//        }
//    }
//
//    // if abbreviated, draw the dots and the last element
//    if (_upLayout->HasHidden())
//    {
//        double dotsXMid = _upLayout->GetDotsMidX();
//        svgDots(os, dotsXMid, _cy);
//    }
//}
//
//void PrintableEmpty::ComputeLayout(const CommandLineArguments& args, double layerLeft, double layerTop)
//{
//    _upLayout = make_unique<LayerLayout>(Size(), layerLeft, args.layerMaxWidth, args.emptyElementStyle.width, args.emptyElementStyle.horizontalSpacing, args.emptyElementStyle.leftPadding, args.emptyElementStyle.rightPadding, args.emptyElementStyle.dotsWidth);
//
//    _layerHeight = args.emptyElementStyle.height + 2 * args.emptyElementStyle.verticalPadding;
//    _cy = layerTop + _layerHeight / 2.0;
//    _elementStyle = args.emptyElementStyle;
//}
//
