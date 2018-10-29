////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GoldenSectionSearch.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace trainers
{
namespace optimization
{
    template<typename FunctionType>
    GoldenSectionMinimizer<FunctionType>::GoldenSectionMinimizer(FunctionType function, double lower, double upper) : _boundary1(lower), _boundary2(upper), _function(std::move(function))
    {
        _minPoint = _goldenComplement * _boundary1 + _golden * _boundary2;
        _minPointValue = _function(_minPoint);
        _boundary1Value = _function(_boundary1);
        _boundary2Value = _function(_boundary2);
    }

    template<typename FunctionType>
    void GoldenSectionMinimizer<FunctionType>::Step(size_t iterations)
    {
        for (size_t i = 0; i < iterations; ++i)
        {
            Step();
        }
    }
    template<typename FunctionType>
    void GoldenSectionMinimizer<FunctionType>::MinimizeToPrecision(double precision)
    {
        do
        {
            Step();
        } while (GetPrecision() > precision);
    }
    template<typename FunctionType>
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
    template<typename FunctionType>
    double GoldenSectionMinimizer<FunctionType>::GetArgMinLowerBound() const
    {
        return std::min(_boundary1, _boundary2);
    }
    template<typename FunctionType>
    double GoldenSectionMinimizer<FunctionType>::GetArgMinUpperBound() const
    {
        return std::max(_boundary1, _boundary2);
    }
    template<typename FunctionType>
    double GoldenSectionMinimizer<FunctionType>::GetApproximateArgMin() const
    {
        return 0.5 * (_boundary1 + _boundary2);
    }

    template<typename FunctionType>
    double GoldenSectionMinimizer<FunctionType>::GetMinUpperBound() const
    {
        return _minPointValue;
    }

    template<typename FunctionType>
    double GoldenSectionMinimizer<FunctionType>::GetMinLowerBound() const
    {
        double min1 = _boundary1Value * (1.0 - 1.0 / _golden) + _minPointValue / _golden;
        double min2 = _boundary2Value * (1.0 - 1.0 / _goldenComplement) + _minPointValue / _goldenComplement;
        return std::min(min1, min2);
    }

    template<typename FunctionType>
    double GoldenSectionMinimizer<FunctionType>::GetPrecision() const
    {
        return GetMinUpperBound() - GetMinLowerBound();
    }
}
}
}
