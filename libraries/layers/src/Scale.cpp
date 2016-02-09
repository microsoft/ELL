////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Scale.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Scale.h"

// stl
#include <string>
#include <functional>

namespace layers
{
    Scale::Scale() : Coordinatewise(std::multiplies<double>(), Type::scale)
    {}

    Scale::Scale(const vector<double>& values, const CoordinateList & coordinates) : Coordinatewise(values, coordinates, std::multiplies<double>(), Type::scale)
    {}
}
