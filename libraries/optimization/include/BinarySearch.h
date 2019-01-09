////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinarySearch.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Interval.h"

#include <cmath>

namespace ell
{
namespace optimization
{
    /// <summary> Parameters for the binary search procedure. </summary>
    struct BinarySearchParameters
    {
        Interval targetInterval; // the interval of target values
        Interval searchInterval; // the search interval
        bool useSearchIntervalValues = false; // tells the binary search to use precomputed SearchIntervalValues and save two calls to the function
        Interval searchIntervalValues = {}; // the values of the function when applied to the boundary of the search interval
    };

    /// <summary> Given a function, a search interval, and a target interval of function values, binary search attempts to find an argument
    /// in the search interval whose function value is in the target interval. The function does not necessarily have to be monotonic, but the search
    /// interval [a,b] should be such that [f(a), f(b)] overlaps with the target interval. If an argument that satisfies the requirements is found,
    /// the current search interval shrinks to length zero (its upper and lower bounds become equal). </summary>
    template <typename FunctionType>
    class BinarySearch
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="function"> A continuous univariate function of the form double(double). </param>
        /// <param name="parameters"> Binary search parameters. </param>
        BinarySearch(FunctionType function, BinarySearchParameters parameters);

        /// <summary> Performs binary search updates until a desired precision is reached or until a max number of function calls is made. </summary>
        void Update(size_t maxFunctionCalls = 1);

        /// <summary> Resets the binary search with new parameters. </summary>
        void Reset(BinarySearchParameters parameters);

        /// <summary> Returns interval an argument whose value is less that or equal to the target value. </summary>
        const Interval& GetCurrentSearchInterval() const { return _searchInterval; }

        /// <summary> Returns an argument whose value is greater or equal to the target value. </summary>
        const Interval& GetCurrentSearchIntervalValues() const { return _searchIntervalValues; }

        /// <summary> Returns true if a point in the interval was found. </summary>
        bool IsSuccessful() const { return _isSuccessful; }

    private:
        FunctionType _function;
        Interval _targetInterval = {};
        Interval _searchInterval = {};
        Interval _searchIntervalValues = {};
        bool _isSuccessful = false;
    };
} // namespace optimization
} // namespace ell

#pragma region implementation

#include "Common.h"

namespace ell
{
namespace optimization
{
    template <typename FunctionType>
    BinarySearch<FunctionType>::BinarySearch(FunctionType function, BinarySearchParameters parameters) :
        _function(std::move(function))
    {
        Reset(parameters);
    }

    template <typename FunctionType>
    void BinarySearch<FunctionType>::Update(size_t maxFunctionCalls)
    {
        if (_isSuccessful)
        {
            return;
        }

        for (size_t i = 0; i < maxFunctionCalls; ++i)
        {
            double candidateArgument = _searchInterval.GetCenter();
            double candidateValue = _function(candidateArgument);

            if (candidateValue <= _targetInterval.End())
            {
                _searchInterval = { candidateArgument, _searchInterval.End() };
                _searchIntervalValues = { candidateValue, _searchIntervalValues.End() };
            }

            if (candidateValue >= _targetInterval.Begin())
            {
                _searchInterval = { _searchInterval.Begin(), candidateArgument };
                _searchIntervalValues = { _searchIntervalValues.Begin(), candidateValue };
            }

            if (_searchInterval.Size() == 0)
            {
                _isSuccessful = true;
                break;
            }
        }
    }

    template <typename FunctionType>
    void BinarySearch<FunctionType>::Reset(BinarySearchParameters parameters)
    {
        _targetInterval = parameters.targetInterval;
        _searchInterval = parameters.searchInterval;

        if (parameters.useSearchIntervalValues)
        {
            _searchIntervalValues = parameters.searchIntervalValues;
            if (_targetInterval.Contains(_searchIntervalValues.Begin()))
            {
                _searchInterval = { _searchInterval.Begin(), _searchInterval.Begin() };
                _searchIntervalValues = { _searchIntervalValues.Begin(), _searchIntervalValues.Begin() };
                _isSuccessful = true;
            }
            else if (_targetInterval.Contains(_searchIntervalValues.End()))
            {
                _searchInterval = { _searchInterval.End(), _searchInterval.End() };
                _searchIntervalValues = { _searchIntervalValues.End(), _searchIntervalValues.End() };
                _isSuccessful = true;
            }
        }
        else
        {
            double bound1 = _function(_searchInterval.Begin());
            if (_targetInterval.Contains(bound1))
            {
                _searchInterval = { _searchInterval.Begin(), _searchInterval.Begin() };
                _searchIntervalValues = { bound1, bound1 };
                _isSuccessful = true;
                return;
            }

            double bound2 = _function(_searchInterval.End());
            if (_targetInterval.Contains(bound2))
            {
                _searchInterval = { _searchInterval.End(), _searchInterval.End() };
                _searchIntervalValues = { bound2, bound2 };
                _isSuccessful = true;
                return;
            }

            _searchIntervalValues = Interval(bound1, bound2);
        }

        // confirm that the target interval is attainable
        if (!_targetInterval.Intersects(_searchIntervalValues))
        {
            throw OptimizationException("target value interval does not intersect with search interval");
        }
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
