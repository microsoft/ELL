// Scale.h
#pragma once

#include "Coordinatewise.h"

namespace layers
{
    class Scale : public Coordinatewise // replace with std::multiplies
    {
    public:

        /// Ctor
        ///
        Scale();

        /// Ctor
        ///
        Scale(const vector<double>& values, const CoordinateList& coordinates);

        /// default virtual destructor
        ///
        virtual ~Scale() = default;

        /// Static function that specifies the coordinatewise operation
        ///
        static double Operation(double value, double input) { return value * input; }
    };
}
