// Scale.cpp

#include "Scale.h"

#include <string>

#include <functional>

namespace layers
{
    Scale::Scale() : Coordinatewise(std::multiplies<double>(), Type::scale)
    {}

    Scale::Scale(const vector<double>& values, const CoordinateList & coordinates) : Coordinatewise(values, coordinates, std::multiplies<double>(), Type::scale)
    {}
}
