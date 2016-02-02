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
        Scale(const vector<double>& values, const CoordinateList& coordinates);

        /// default virtual destructor
        ///
        virtual ~Scale() = default;
    };
}
