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
    std::vector<double> BinaryErrorAggregator::Result::GetValues() const
    {
        double precision = sumTruePositives == 0.0 ? 0.0 : sumTruePositives / (sumTruePositives + sumFalsePositives);
        double recall = sumTruePositives == 0.0 ? 0.0 : sumTruePositives / (sumTruePositives + sumFalseNegatives);
        double f1 = (precision + recall) == 0.0 ? 0.0 : 2 * (precision * recall) / (precision + recall);

        return {GetErrorRate(), precision, recall, f1};
    }

    double BinaryErrorAggregator::Result::GetErrorRate() const
    {
        double allFalse = sumFalsePositives + sumFalseNegatives;
        double allTrue = sumTruePositives + sumTrueNegatives;
        return allFalse == 0.0 ? 0.0 : allFalse / (allTrue + allFalse);
    }

    void BinaryErrorAggregator::Update(double prediction, double label, double weight)
    {
        if (label > 0)
        {
            if (prediction > 0)
            {
                _result.sumTruePositives += weight;
            }
            else
            {
                _result.sumFalseNegatives += weight;
            }
        }
        else
        {
            if (prediction >= 0)
            {
                _result.sumFalsePositives += weight;
            }
            else
            {
                _result.sumTrueNegatives += weight;
            }
        }
    }

    void BinaryErrorAggregator::Reset()
    {
        _result = Result();
    }

    std::vector<std::string> BinaryErrorAggregator::GetValueNames() const
    { 
        return {"ErrorRate", "Precision", "Recall", "F1-Score"}; 
    }
}
