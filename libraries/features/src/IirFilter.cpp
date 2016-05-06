////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [project]
//  File:     IirFilter.cpp (features)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IirFilter.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace features
{
    IirFilter::IirFilter(std::vector<double> b, std::vector<double> a) : _valid(false), _previousOutput(a.size()), _previousInput(b.size()), _a(a), _b(b)
    {
    }

    double IirFilter::FilterSample(double x) const
    {
        if (_previousInput.Size() == 0 || !_valid)
        {
            for (int index = 0; index < _b.size() - 1; index++)
            {
                _previousInput.Append(x);
            }
            _valid = true;
        }

        _previousInput.Append(x);
        double output = 0;
        for (unsigned int index = 0; index < std::min(_previousOutput.Size(), _a.size() - 1); index++)
        {
            output -= _a[index + 1] * _previousOutput[index];
        }

        for (unsigned int index = 0; index < std::min(_previousInput.Size(), _b.size()); index++)
        {
            output += _b[index] * _previousInput[index];
        }

        _previousOutput.Append(output);
        assert(_a[0] != 0);
        assert(std::isfinite(output));
        return output / _a[0];
    }

    void IirFilter::Reset()
    {
        _valid = false;
    }
}