// Shift.cpp

#include "Shift.h"

// stl
#include <string>

#include <functional>

namespace layers
{
    Shift::Shift() : Coordinatewise(std::plus<double>(), Type::shift)
    {}

    Shift::Shift(double value, Coordinate coordinate) : Coordinatewise(value, coordinate, std::plus<double>(), Type::shift)
    {}
    
    Shift::Shift(const vector<double>& values, const CoordinateList & coordinates) : Coordinatewise(values, coordinates, std::plus<double>(), Type::shift)
    {}
}
