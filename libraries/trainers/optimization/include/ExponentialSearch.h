////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExponentialSearch.h (optimization)
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
        /// <summary> Parameters for exponential search. </summary>
        struct ExponentialSearchParameters
        {
            Interval targetInterval; // the exponential search can return any value in this interval
            double argumentGuess = 0.0; // where to start the search, a good guess of an argument whose value falls in the target interval
            double base = 2.0; // the base of the exponent, setting this to 2 means that the search interval doubles on each iteration
        };

        /// <summary> Given a monotonic function and a target interval of function values, exponential search attempts to find an interval
        /// of arguments that contains at least one argument whose function value is contained in the target interval. </summary>
        template <typename FunctionType>
        class ExponentialSearch
        {
        public:
            /// <summary> Constructor. </summary>
            ExponentialSearch(FunctionType function, ExponentialSearchParameters parameters);

            /// <summary> Performs exponential search updates until an acceptable target interval is found or until a max number of function calls is made. </summary>
            void Update(size_t maxFunctionCalls = 1);

            /// <summary> Resets the exponential search with new parameters. </summary>
            void Reset(ExponentialSearchParameters parameters);

            /// <summary> Returns the interval of arguments that contains an answer. </summary>
            const Interval& GetBoundingArguments() const { return _boundingArguments; }

            /// <summary> Returns the interval of values attained by the bounding argument. This interval always intersects with the target interval. </summary>
            const Interval& GetBoundingValues() const { return _boundingValues; }

            /// <summary> Returns true if a bounding interval has been found. </summary>
            bool IsSuccessful() const { return _isSuccessful; }

        private:
            void CheckNewArgumentShift();

            FunctionType _function;
            Interval _targetInterval = {};
            Interval _boundingArguments = {};
            Interval _boundingValues = {};

            double _initialArgument = 0;
            double _base = 0;
            double _argumentShift = 0;
            double _currentArgument = 0;
            double _currentValue = 0;
            bool _isSuccessful = false;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma region implementation

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename FunctionType>
        ExponentialSearch<FunctionType>::ExponentialSearch(FunctionType function, ExponentialSearchParameters parameters) :
            _function(std::move(function))
        {
            Reset(parameters);
        }

        template <typename FunctionType>
        void ExponentialSearch<FunctionType>::Update(size_t maxFunctionCalls)
        {
            for (size_t i = 0; i < maxFunctionCalls && !_isSuccessful; ++i)
            {
                _argumentShift *= _base;
                CheckNewArgumentShift();
            }
        }

        template <typename FunctionType>
        void ExponentialSearch<FunctionType>::Reset(ExponentialSearchParameters parameters)
        {
            // initialize
            _targetInterval = parameters.targetInterval;
            _currentArgument = _initialArgument = parameters.argumentGuess;
            _base = parameters.base;
            _currentValue = _function(_initialArgument);

            // if we're lucky, we found the answer
            if (_targetInterval.Contains(_currentValue))
            {
                _boundingArguments = { _currentArgument, _currentArgument };
                _boundingValues = { _currentValue, _currentValue };
                _isSuccessful = true;
                return;
            }

            _argumentShift = 1.0;
            if (_currentValue > _targetInterval.End())
            {
                _argumentShift = -1.0;
            }
            CheckNewArgumentShift();
        }

        template <typename FunctionType>
        void ExponentialSearch<FunctionType>::CheckNewArgumentShift()
        {
            // find new candidate
            double candidateArgument = _initialArgument + _argumentShift;
            double candidateValue = _function(candidateArgument);

            // if we're lucky, we found the answer
            if (_targetInterval.Contains(candidateValue))
            {
                _boundingArguments = { candidateArgument, candidateArgument };
                _boundingValues = { candidateValue, candidateValue };
                _isSuccessful = true;
                return;
            }

            // update the bounding intervals
            Interval candidateValueInterval(_currentValue, candidateValue);
            if (_targetInterval.Intersects(candidateValueInterval))
            {
                _boundingArguments = { _currentArgument, candidateArgument };
                _boundingValues = candidateValueInterval;
                _isSuccessful = true;
            }
            else
            {
                _boundingArguments = { candidateArgument, _argumentShift * std::numeric_limits<double>::infinity() };
                _boundingValues = { candidateValue, _argumentShift * std::numeric_limits<double>::infinity() };
            }

            _currentArgument = candidateArgument;
            _currentValue = candidateValue;
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
