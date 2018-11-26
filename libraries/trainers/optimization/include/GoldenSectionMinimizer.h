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

#pragma region implementation

#pragma once

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename FunctionType>
        GoldenSectionMinimizer<FunctionType>::GoldenSectionMinimizer(FunctionType function, double lower, double upper) :
            _boundary1(lower),
            _boundary2(upper),
            _function(std::move(function))
        {
            _minPoint = _goldenComplement * _boundary1 + _golden * _boundary2;
            _minPointValue = _function(_minPoint);
            _boundary1Value = _function(_boundary1);
            _boundary2Value = _function(_boundary2);
        }

        template <typename FunctionType>
        void GoldenSectionMinimizer<FunctionType>::Step(size_t iterations)
        {
            for (size_t i = 0; i < iterations; ++i)
            {
                Step();
            }
        }
        template <typename FunctionType>
        void GoldenSectionMinimizer<FunctionType>::MinimizeToPrecision(double precision)
        {
            do
            {
                Step();
            } while (GetPrecision() > precision);
        }
        template <typename FunctionType>
        void GoldenSectionMinimizer<FunctionType>::Step()
        {
            double newPoint = _goldenComplement * _boundary1 + _golden * _minPoint;
            double newPointValue = _function(newPoint);
            if (newPointValue < _minPointValue)
            {
                _boundary2 = _minPoint;
                _boundary2Value = _minPointValue;
                _minPoint = newPoint;
                _minPointValue = newPointValue;
            }
            else
            {
                _boundary1 = _boundary2;
                _boundary1Value = _boundary2Value;
                _boundary2 = newPoint;
                _boundary2Value = newPointValue;
            }
        }
        template <typename FunctionType>
        double GoldenSectionMinimizer<FunctionType>::GetArgMinLowerBound() const
        {
            return std::min(_boundary1, _boundary2);
        }
        template <typename FunctionType>
        double GoldenSectionMinimizer<FunctionType>::GetArgMinUpperBound() const
        {
            return std::max(_boundary1, _boundary2);
        }
        template <typename FunctionType>
        double GoldenSectionMinimizer<FunctionType>::GetApproximateArgMin() const
        {
            return 0.5 * (_boundary1 + _boundary2);
        }

        template <typename FunctionType>
        double GoldenSectionMinimizer<FunctionType>::GetMinUpperBound() const
        {
            return _minPointValue;
        }

        template <typename FunctionType>
        double GoldenSectionMinimizer<FunctionType>::GetMinLowerBound() const
        {
            double min1 = _boundary1Value * (1.0 - 1.0 / _golden) + _minPointValue / _golden;
            double min2 = _boundary2Value * (1.0 - 1.0 / _goldenComplement) + _minPointValue / _goldenComplement;
            return std::min(min1, min2);
        }

        template <typename FunctionType>
        double GoldenSectionMinimizer<FunctionType>::GetPrecision() const
        {
            return GetMinUpperBound() - GetMinLowerBound();
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
