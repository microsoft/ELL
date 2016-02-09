////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Shift.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Coordinatewise.h"

namespace layers
{
    class Shift : public Coordinatewise // replace with std::plus
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

        /// default virtual destructor
        ///
        virtual ~Shift() = default;
    };
}
