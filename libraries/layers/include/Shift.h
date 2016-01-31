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
        Shift(double value, Coordinate coordinate);

        /// Ctor
        ///
        Shift(const vector<double>& values, const CoordinateList& coordinates);
    };
}
