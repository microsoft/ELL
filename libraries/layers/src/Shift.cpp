// Shift.cpp

#include "Shift.h"

#include <string>
using std::to_string;

namespace layers
{
    Shift::Shift() : Coordinatewise(Operation, Type::shift)
    {}

    Shift::Shift(double value, Coordinate coordinate) : Coordinatewise(value, coordinate, Operation, Type::shift)
    {}
    
    Shift::Shift(const vector<double>& values, const CoordinateList & coordinates) : Coordinatewise(values, coordinates, Operation, Type::shift)
    {}
}
