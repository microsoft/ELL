////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Interval.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <algorithm>

namespace ell
{
namespace optimization
{
    /// <summary> Represents a closed interval of reals. </summary>
    class Interval
    {
    public:
        /// <summary> Default constructor. </summary>
        Interval() = default;

        /// <summary> Constructor. </summary>
        Interval(double boundary1, double boundary2);

        /// <summary> Returns the smallest value in the interval. </summary>
        double Begin() const { return _begin; }

        /// <summary> Returns the largest value in the interval. </summary>
        double End() const { return _end; }

        /// <summary> Returns the center of the interval. </summary>
        double GetCenter() const;

        /// <summary> Returns the size of the interval. </summary>
        double Size() const;

        /// <summary> Returns true if the interval containes a given value. </summary>
        bool Contains(double value) const;

        /// <summary> Returns true if the interval intersects with another interval. </summary>
        bool Intersects(const Interval& other) const;

    private:
        double _begin = 0;
        double _end = 0;
    };
} // namespace optimization
} // namespace ell
