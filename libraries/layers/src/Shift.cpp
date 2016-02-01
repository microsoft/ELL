// Shift.cpp

#include "Shift.h"

// stl
#include <string>
using std::to_string;

#include <functional>
using std::plus;

namespace layers
{
    Shift::Shift() : Coordinatewise(plus<double>(), Type::shift)
    {}

    Shift::Shift(double value, Coordinate coordinate) : Coordinatewise(value, coordinate, plus<double>(), Type::shift)
    {}
    
    Shift::Shift(const vector<double>& values, const CoordinateList & coordinates) : Coordinatewise(values, coordinates, plus<double>(), Type::shift)
    {}
}
