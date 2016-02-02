// Scale.cpp

#include "Scale.h"

#include <string>
using std::to_string;

#include <functional>
using std::multiplies;

namespace layers
{
    Scale::Scale() : Coordinatewise(multiplies<double>(), Type::scale)
    {}

    Scale::Scale(const vector<double>& values, const CoordinateList & coordinates) : Coordinatewise(values, coordinates, multiplies<double>(), Type::scale)
    {}
}
