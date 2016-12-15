////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryErrorAggregator.cpp (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryErrorAggregator.h"

namespace ell
{
namespace evaluators
{
    void BinaryErrorAggregator::Update(double prediction, double label, double weight)
    {
        if (label > 0)
        {
            if (prediction > 0)
            {
                _sumTruePositives += weight;
            }
            else
            {
                _sumFalseNegatives += weight;
            }
        }
        else
        {
            if (prediction >= 0)
            {
                _sumFalsePositives += weight;
            }
            else
            {
                _sumTrueNegatives += weight;
            }
        }
    }

    std::vector<double> BinaryErrorAggregator::GetResult() const
    {
        double allFalse = _sumFalsePositives + _sumFalseNegatives;
        double allTrue = _sumTruePositives + _sumTrueNegatives;
        double errorRate = allFalse == 0.0 ? 0.0 : allFalse / (allTrue + allFalse);

        double precision = _sumTruePositives == 0.0 ? 0.0 : _sumTruePositives / (_sumTruePositives + _sumFalsePositives);
        double recall = _sumTruePositives == 0.0 ? 0.0 : _sumTruePositives / (_sumTruePositives + _sumFalseNegatives);
        double f1 = (precision + recall) == 0.0 ? 0.0 : 2 * (precision * recall) / (precision + recall);

        return { errorRate, precision, recall, f1 };
    }

    void BinaryErrorAggregator::Reset()
    {
        _sumTruePositives = 0.0;
        _sumTrueNegatives = 0.0;
        _sumFalsePositives = 0.0;
        _sumFalseNegatives = 0.0;
    }

    std::vector<std::string> BinaryErrorAggregator::GetValueNames() const
    {
        return { "ErrorRate", "Precision", "Recall", "F1-Score" };
    }
}
}
