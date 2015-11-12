// Scale.cpp

#include "Scale.h"
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::to_string;

namespace mappings
{
    Scale::Scale() : Coordinatewise(Scale::Multiply)
    {
        _type = types::Scale;
    }

    double Scale::Multiply(double a, double b)
    {
        return a * b;
    }
}
