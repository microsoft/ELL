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
    /// <summary> A predictor that ignores its input and outputs a constant number. This class is used to define decision trees. </summary>
    class ConstantPredictor
    {
    public:

        /// <summary> Constructs an instance of ConstantPredictor. </summary>
        ///
        /// <param name="value"> The constant output value. </param>
        ConstantPredictor(double value);

        /// <summary> A function that ignores its input and returns a constant value. </summary>
        /// 
        /// <returns> A constant value. </returns>
        template<typename AnyType>
        double Compute(const AnyType&) const { return _value; }

        /// <summary> Gets the constant value. </summary>
        ///
        /// <returns> The constant value. </returns>
        double GetValue() const { return _value; }

    private:
        double _value;
    };
}