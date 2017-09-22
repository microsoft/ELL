////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorStats.h (print)
//  Authors:  Charles Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>
#include <cstring> // for size_t
#include <vector>

class VectorStats
{
public:
    VectorStats() = default;

    template <typename ValueType>
    VectorStats(const std::vector<ValueType>& vec);

    size_t NumElements() const { return _size; }
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
    size_t _size = 0;
    double _min = 0;
    double _max = 0;
    double _mean = 0;
    double _variance = 0;
    double _stdDev = 0;
};

template <typename ValueType>
std::vector<ValueType> Subtract(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

template <typename ValueType>
std::vector<ValueType> Abs(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

#include "../tcc/VectorStats.tcc"