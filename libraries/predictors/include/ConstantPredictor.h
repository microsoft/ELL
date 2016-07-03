////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace predictors
{
    class ConstantPredictor
    {
        ConstantPredictor(double value);

        template<typename AnyType>
        double Compute(const AnyType&) const { return _value; }

        double GetValue() const { return _value; }

    private:
        double _value;
    };
}