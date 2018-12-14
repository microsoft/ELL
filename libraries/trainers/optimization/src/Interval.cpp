////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Interval.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Interval.h"

#include <algorithm>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        Interval::Interval(double boundary1, double boundary2)
        {
            _begin = std::min(boundary1, boundary2);
            _end = std::max(boundary1, boundary2);
        }

        double Interval::GetCenter() const
        {
            return 0.5 * (_end + _begin);
        }

        double Interval::Size() const
        {
            return _end - _begin;
        }

        bool Interval::Contains(double value) const
        {
            return (_begin <= value && value <= _end);
        }

        bool Interval::Intersects(const Interval& other) const
        {
            return !(_end < other.Begin() || other.End() < _begin);
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell
