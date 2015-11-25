// Scale.h
#pragma once

#include "Coordinatewise2.h"

namespace mappings
{
    class Scale : public Coordinatewise2
    {
    public:

        /// Constructs a default Mapping
        ///
        Scale();

        /// Constructs a Shift Mapping from index value pairs
        ///
        template <typename IndexValueIteratorType>
        Scale(IndexValueIteratorType indexValueIterator);

        static double Multiply(double a, double b);
    };
}

#include "../tcc/Scale.tcc"
