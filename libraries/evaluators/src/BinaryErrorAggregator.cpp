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
    void BinaryErrorAggregator::Update(double prediction, double label, double weight)
    {
        if (label > 0)
        {
            if (prediction > 0)
            {
                _sumWeightedTruePositives += weight;
            }
            else
            {
                _sumWeightedFalseNegatives += weight;
            }
        }
        else
        {
            if (prediction > 0)
            {
                _sumWeightedFalsePositives += weight;
            }
            else
            {
                _sumWeightedTrueNegatives += weight;
            }
        }
    }

    void BinaryErrorAggregator::Print(std::ostream & os) const
    {
        double sumWeightFalse = _sumWeightedFalsePositives + _sumWeightedFalseNegatives;
        double sumWeightTrue = _sumWeightedTruePositives + _sumWeightedTrueNegatives;
        double errorRate = sumWeightFalse / (sumWeightTrue + sumWeightFalse);
        os << errorRate << "\t";
    }
}