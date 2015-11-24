// Shift.cpp

#include "Shift.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Shift::Shift() : Coordinatewise(Shift::Add)
    {
        _type = Types::shift;
    }

    double Shift::Add(double a, double b)
    {
        return a + b;
    }
}
