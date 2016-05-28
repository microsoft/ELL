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
        double precision = truePositives == 0.0 ? 0.0 : truePositives / (truePositives + falsePositives);
        double recall = truePositives == 0.0 ? 0.0 : truePositives / (truePositives + falseNegatives);
        double f1 = (precision + recall) == 0.0 ? 0.0 : 2 * (precision * recall) / (precision + recall);

        return {GetErrorRate(), precision, recall, f1};
    }

    double BinaryErrorAggregator::Value::GetErrorRate() const
    {
        double allFalse = falsePositives + falseNegatives;
        double allTrue = truePositives + trueNegatives;
        return allFalse == 0.0 ? 0.0 : allFalse / (allTrue + allFalse);
    }

    void BinaryErrorAggregator::Update(double prediction, double label, double weight)
    {
        if (label > 0)
        {
            if (prediction > 0)
            {
                _value.sumTruePositives += weight;
            }
            else
            {
                _value.sumFalseNegatives += weight;
            }
        }
        else
        {
            if (prediction >= 0)
            {
                _value.sumFalsePositives += weight;
            }
            else
            {
                _value.sumTrueNegatives += weight;
            }
        }
    }

    BinaryErrorAggregator::Value BinaryErrorAggregator::GetAndReset()
    {
        Value newValue;
        std::swap(_value, newValue);
        return newValue;
    }

    std::vector<std::string> BinaryErrorAggregator::GetValueNames() const 
    { 
        return {"ErrorRate", "Precision", "Recall", "F1-Score"}; 
    }
}