// Shift.h
#pragma once

#include "Coordinatewise.h"

namespace mappings
{
    class Shift : public Coordinatewise
    {
    public:

        /// Constructs a default Mapping
        ///
        Shift();

        /// Constructs a Shift Mapping from index value pairs
        ///
        template <typename IndexValueIterator>
        Shift(IndexValueIterator begin, IndexValueIterator end);

        static double Add(double a, double b);
    };
}

#include "../tcc/Shift.tcc"
