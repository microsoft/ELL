////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareArguments.h (print)
//  Authors:  Charles Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>
#include <vector>

class VectorStats
{
public:
    VectorStats() = default;

    template <typename ValueType>
    VectorStats(const std::vector<ValueType>& vec);

    double Min() const { return _min; }
    double Max() const { return _max; }
    double Mean() const { return _mean; }
    double Variance() const { return _variance; }
    double StdDev() const { return _stdDev; }
    bool IsValid() const { return _valid; }

    template <typename ValueType>
    static double Diff(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

private:
    bool _valid = false;
    double _min = 0;
    double _max = 0;
    double _mean = 0;
    double _variance = 0;
    double _stdDev = 0;
};

#include "../tcc/VectorStats.tcc"