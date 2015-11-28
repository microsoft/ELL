// Scale.h
#pragma once

#include "Coordinatewise.h"

namespace layers
{
    class Scale : public Coordinatewise
    {
    public:

        /// Ctor
        ///
        Scale();

        /// Ctor
        ///
        Scale(const vector<double>& values, const vector<Coordinate>& coordinates);

        /// Static function that specifies the coordinatewise operation
        ///
        static double Operation(double value, double input) { return value * input; }
    };
}
