// Scale.cpp

#include "Scale.h"

#include <string>
using std::to_string;

namespace mappings
{
    Scale::Scale() : Coordinatewise(Operation, Type::scale)
    {}

    Scale::Scale(const vector<double>& values, const vector<Coordinate> & coordinates) : Coordinatewise(values, coordinates, Operation, Type::scale)
    {}
}
