////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GoldenSectionSearch.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.h"

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Implements the Golden Section Search minimization procedure for a univariate quasiconvex function.</summary>
        template <typename FunctionType>
        class GoldenSectionMinimizer
        {
        public:
            /// <summary> Constructor.</summary>
            ///
            /// <param name="function"> A quasiconvex function of the form double(double). </param>
            /// <param name="lower"> A lower bound on the argmin.</param>
            /// <param name="upper"> An upper bound on the argmin.</param>
            GoldenSectionMinimizer(FunctionType function, double lower, double upper);

            /// <summary> Performs one iteration of the algorithm.</summary>
            void Step();

            /// <summary> Performs multiple iterations of the algorithm.</summary>
            void Step(size_t iterations);

            /// <summary> Performs multiple iterations of the algorithm, until a certain precision is reached (only valid for convex functions).</summary>
            void MinimizeToPrecision(double precision);

            /// <summary> Returns a lower bound on the argmin.</summary>
            double GetArgMinLowerBound() const;

            /// <summary> Returns an upper bound on the argmin.</summary>
            double GetArgMinUpperBound() const;

            /// <summary> Returns an approximate argmin, the mean of the lower and upper bounds.</summary>
            double GetApproximateArgMin() const;

            /// <summary> Returns an upper bound on the min.</summary>
            double GetMinUpperBound() const;

            /// <summary> Returns a lower bound on the min (only valid for convex functions).</summary>
            double GetMinLowerBound() const;

            /// <summary> Returns the additive error of the current approximate min.</summary>
            double GetPrecision() const;

        private:
            const double _golden = 0.618033988;
            const double _goldenComplement = 1.0 - _golden;
            double _boundary1;
            double _boundary2;
            double _minPoint;
            double _boundary1Value;
            double _boundary2Value;
            double _minPointValue;
            FunctionType _function;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#include "../tcc/GoldenSectionMinimizer.tcc"
