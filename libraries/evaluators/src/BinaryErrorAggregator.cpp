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
        double allFalse = falsePositives + falseNegatives;
        double allTrue = truePositives + trueNegatives;
        double errorRate = allFalse / (allTrue + allFalse);
        double precision = truePositives / (truePositives + falsePositives);
        double recall = truePositives / (truePositives + falseNegatives);
        return std::to_string(errorRate) + '\t' + std::to_string(precision) + '\t' + std::to_string(recall);
    }

    void BinaryErrorAggregator::Update(double prediction, double label, double weight)
    {
        if (label > 0)
        {
            if (prediction > 0)
            {
                _value.truePositives += weight;
            }
            else
            {
                _value.falseNegatives += weight;
            }
        }
        else
        {
            if (prediction > 0)
            {
                _value.falsePositives += weight;
            }
            else
            {
                _value.trueNegatives += weight;
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