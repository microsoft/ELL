// Scale.h
#pragma once

#include "Coordinatewise.h"

namespace mappings
{
    class Scale : public Coordinatewise
    {
    public:

        /// Constructs a default Mapping
        ///
        Scale();

        /// Constructs a Shift Mapping from index value pairs
        ///
        template <typename IndexValueIterator>
        Scale(IndexValueIterator begin, IndexValueIterator end);

        static double Multiply(double a, double b);
    };
}

#include "../tcc/Scale.tcc"
