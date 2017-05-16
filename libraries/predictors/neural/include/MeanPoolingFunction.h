////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanPoolingFunction.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// size_t
#include <cstddef>

// STL
#include <limits>

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the average function used in poolng layers. </summary>
    class MeanPoolingFunction
    {
    public:

        /// <summary> Instantiates an instance of this function with all accumulation data cleared. </summary>
        MeanPoolingFunction();

        /// <summary> Adds another value for consideration. </summary>
        ///
        /// <param name="input"> The input value. </param>
        void Accumulate(double value);

        /// <summary> Returns the value of this function. </summary>
        ///
        /// <return> The average value from all accumulated values. </return>
        double GetValue() const;

        /// <summary> Value to accumulate when on padding. </summary>
        ///
        /// <return> The value when accumulate is called on padding. </return>
        double GetValueAtPadding() const { return 0; }

    private:
        double _sum;
        size_t _numValues;
    };
}
}
}
