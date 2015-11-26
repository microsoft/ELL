// Shift.cpp

#include "Shift.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Shift::Shift() : Coordinatewise(Operation)
    {
        _type = Types::shift;
    }

    Shift::Shift(const vector<double>& values, const IndexPairList & coordinates) : Coordinatewise(values, coordinates, Operation)
    {
        _type = Types::shift;
    }
}
