// Scale.cpp

#include "Scale.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Scale::Scale() : Coordinatewise(Operation)
    {
        _type = Types::scale;
    }

    Scale::Scale(const vector<double>& values, const IndexPairList & coordinates) : Coordinatewise(values, coordinates, Operation)
    {
        _type = Types::scale;
    }
}
