// Shift.cpp

#include "Shift.h"

#include <string>
using std::to_string;

namespace mappings
{
    Shift::Shift() : Coordinatewise(Operation, Type::shift)
    {}

    Shift::Shift(const vector<double>& values, const vector<Coordinate> & coordinates) : Coordinatewise(values, coordinates, Operation, Type::shift)
    {}
}
