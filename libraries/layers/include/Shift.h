// Shift.h
#pragma once

#include "Coordinatewise.h"

namespace layers
{
    class Shift : public Coordinatewise
    {
    public:

        /// Ctor
        ///
        Shift();

        /// Ctor
        ///
        Shift(const vector<double>& values, const vector<Coordinate>& coordinates);

        /// Static function that specifies the coordinatewise operation
        ///
        static double Operation(double value, double input) { return value + input; }
    };
}
