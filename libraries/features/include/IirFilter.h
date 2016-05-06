////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [project]
//  File:     IirFilter.h (features)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RingBuffer.h"

#include <vector>

namespace features
{
    class IirFilter
    {
    public:
        IirFilter(std::vector<double> b, std::vector<double> a); // a are the coeffs on past output values (feedback), and b are the coeffs on input values (feedforward / FIR part)
        double FilterSample(double x) const;
        void Reset();

    private:
        // Output is computed according to this equation:
        // a0*y[t] + a1*y[t-1] + a2*y[t-2] + ... = b0*x[t] + b1*x[t-1] + b2*x[t-2] -->
        // y[t] = (b0*x[t] + b1*x[t-1] + b2*x[t-2] ... - a1*y[t-1] - a2*y[t-2] + ...) / a0
        mutable bool _valid = false;
        mutable RingBuffer<double> _previousOutput; // y[n]
        mutable RingBuffer<double> _previousInput; // x[n]
        std::vector<double> _a; // _a[0] == a1 (since we never really use the scaling coeff a0)
        std::vector<double> _b; // _b[0] == b0, the scaling on the current input
    };
}
