////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Range.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iosfwd>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// A class representing the half-open interval `[begin, end)`, with an increment between points of _increment.
        /// </summary>
        class Range
        {
        public:
            Range(int begin, int end, int increment = 1);

            int Begin() const;
            int End() const;
            int Size() const;
            int Increment() const;

        private:
            int _begin;
            int _end;
            int _increment;
        };

        bool operator==(const Range& i1, const Range& i2);
        bool operator!=(const Range& i1, const Range& i2);
        bool operator<(const Range& i1, const Range& i2);
        std::ostream& operator<<(std::ostream& os, const Range& r);
    } // namespace loopnests
} // namespace value
} // namespace ell
