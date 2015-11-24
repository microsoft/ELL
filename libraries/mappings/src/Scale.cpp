// Scale.cpp

#include "Scale.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Scale::Scale() : Coordinatewise(Scale::Multiply)
    {
        _type = Types::scale;
    }

    double Scale::Multiply(double a, double b)
    {
        return a * b;
    }
}
