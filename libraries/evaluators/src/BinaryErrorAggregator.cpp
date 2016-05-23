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
    std::vector<double> BinaryErrorAggregator::Value::GetValues() const
    {
        double allFalse = falsePositives + falseNegatives;
        double allTrue = truePositives + trueNegatives;

        double errorRate = allFalse / (allTrue + allFalse);
        double precision = truePositives == 0.0 ? 0.0 : truePositives / (truePositives + falsePositives);
        double recall = truePositives == 0.0 ? 0.0 : truePositives / (truePositives + falseNegatives);
        double f1 = (precision + recall) == 0.0 ? 0.0 : 2 * (precision * recall) / (precision + recall);

        return {errorRate, precision, recall, f1};
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
            if (prediction >= 0)
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

    std::vector<std::string> BinaryErrorAggregator::GetHeader() const 
    { 
        return {"ErrorRate", "Precision", "Recall", "F1-Score"}; 
    }
}