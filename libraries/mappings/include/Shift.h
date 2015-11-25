// Shift.h
#pragma once

#include "Coordinatewise2.h"

namespace mappings
{
    class Shift : public Coordinatewise2
    {
    public:

        /// Constructs a default Mapping
        ///
        Shift();

        /// Constructs a Shift Mapping from index value pairs
        ///
        template <typename IndexValueIteratorType>
        Shift(IndexValueIteratorType begin, IndexValueIteratorType end);

        static double Add(double a, double b);
    };
}

#include "../tcc/Shift.tcc"
