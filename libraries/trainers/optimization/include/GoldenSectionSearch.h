////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GoldenSectionSearch.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.h"
#include "Interval.h"

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        struct GoldenSectionSearchParameters
        {
            Interval interval; // the search interval
            double earlyExitIntervalWidth = 0; // stop updating if the interval is smaller than this value
            Objective objective = Objective::minimize; // are we minimizing or maximizing
        };

        /// <summary> Implements the Golden Section Search optimization procedure for a univariate quasiconvex function. </summary>
        template <typename FunctionType>
        class GoldenSectionSearch
        {
        public:
            /// <summary> Constructor. </summary>
            ///
            /// <param name="function"> A quasiconvex function of the form double(double). </param>
            /// <param name="parameters"> Search parameters. </param>
            GoldenSectionSearch(FunctionType function, GoldenSectionSearchParameters parameters);

            /// <summary> Performs multiple iterations of the algorithm. </summary>
            void Update(size_t maxFunctionCalls);

            /// <summary> Resets the golden section search with new parameters. </summary>
            void Reset(GoldenSectionSearchParameters parameters);

            /// <summary> Returns the current search interval. </summary>
            Interval GetCurrentInterval() const;

            /// <summary> Returns an approximate optimizer. </summary>
            double GetBestArgument() const { return _current; }

            /// <summary> Returns the approximate optimum. </summary>
            double GetBestValue() const { return _currentValue; }

            /// <summary> Returns true if the search interval is below the early exit interval width. </summary>
            bool IsSuccessful() const { return _isSuccessful; }

        private:
            FunctionType _function;
            double _optimizationDirectionMultiplier = 1.0; // -1.0 for minimization, 1.0 for maximization
            double _earlyExitIntervalWidth = 0;
            double _boundary1 = 0;
            double _boundary2 = 0;
            double _current = 0;
            double _currentValue = 0;
            bool _isSuccessful = false;

            const double _golden = 0.618033988;
            const double _goldenComplement = 1.0 - _golden;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma region implementation

#include <algorithm>
#include <cassert>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename FunctionType>
        GoldenSectionSearch<FunctionType>::GoldenSectionSearch(FunctionType function, GoldenSectionSearchParameters parameters) :
            _function(std::move(function))
        {
            Reset(parameters);
        }

        template <typename FunctionType>
        void GoldenSectionSearch<FunctionType>::Update(size_t maxFunctionCalls)
        {
            if (_isSuccessful)
            {
                return;
            }

            for (size_t i = 0; i < maxFunctionCalls; ++i)
            {
                double candidate = _goldenComplement * _boundary1 + _golden * _current;

                assert(std::abs(candidate - _boundary2 - _boundary1 + _current) < 1.0e-6);

                double candidateValue = _function(candidate);

                // if candidate is better than boundary points and current point
                if (_optimizationDirectionMultiplier * (candidateValue - _currentValue) > 0)
                {
                    _boundary2 = _current;
                    _current = candidate;
                    _currentValue = candidateValue;
                }

                // if candidate is better than boundary points but worse than current point
                else
                {
                    _boundary1 = _boundary2;
                    _boundary2 = candidate;
                }

                if (std::abs(_boundary1 - _boundary2) <= _earlyExitIntervalWidth)
                {
                    _isSuccessful = true;
                    break;
                }
            }
        }

        template <typename FunctionType>
        void GoldenSectionSearch<FunctionType>::Reset(GoldenSectionSearchParameters parameters)
        {
            _isSuccessful = false;

            if (parameters.objective == Objective::minimize)
            {
                _optimizationDirectionMultiplier = -1;
            }
            else
            {
                _optimizationDirectionMultiplier = 1;
            }

            _earlyExitIntervalWidth = parameters.earlyExitIntervalWidth;
            _boundary1 = parameters.interval.Begin();
            _boundary2 = parameters.interval.End();
            _current = _goldenComplement * _boundary1 + _golden * _boundary2;

            _currentValue = _function(_current);

            if (parameters.interval.Size() <= _earlyExitIntervalWidth)
            {
                _isSuccessful = true;
            }
        }

        template <typename FunctionType>
        Interval GoldenSectionSearch<FunctionType>::GetCurrentInterval() const
        {
            return Interval { _boundary1, _boundary2 };
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
