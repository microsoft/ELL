////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryErrorAggregator.cpp (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryErrorAggregator.h"

namespace evaluators
{
    std::string BinaryErrorAggregator::Value::ToString() const 
    {
        double sumWeightFalse = sumWeightedFalsePositives + sumWeightedFalseNegatives;
        double sumWeightTrue = sumWeightedTruePositives + sumWeightedTrueNegatives;
        double errorRate = sumWeightFalse / (sumWeightTrue + sumWeightFalse);
        return std::to_string(errorRate);
    }

    void BinaryErrorAggregator::Update(double prediction, double label, double weight)
    {
        if (label > 0)
        {
            if (prediction > 0)
            {
                _value.sumWeightedTruePositives += weight;
            }
            else
            {
                _value.sumWeightedFalseNegatives += weight;
            }
        }
        else
        {
            if (prediction > 0)
            {
                _value.sumWeightedFalsePositives += weight;
            }
            else
            {
                _value.sumWeightedTrueNegatives += weight;
            }
        }
    }

    BinaryErrorAggregator::Value BinaryErrorAggregator::GetAndReset()
    {
        Value newValue;
        std::swap(_value, newValue);
        return newValue;
    }
}