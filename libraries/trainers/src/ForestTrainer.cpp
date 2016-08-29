////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainer.h"

// utilites
#include "Exception.h"

namespace trainers
{
    void ForestTrainerBase::Sums::Increment(const dataset::WeightLabel& weightLabel)
    {
        sumWeights += weightLabel.weight;
        sumWeightedLabels += weightLabel.weight * weightLabel.label;
    }

    typename ForestTrainerBase::Sums ForestTrainerBase::Sums::operator-(const Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    double ForestTrainerBase::Sums::GetMeanLabel() const
    {
        if(sumWeights == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "Can't compute mean because sum of weights equals zero");
        }
        return sumWeightedLabels / sumWeights;
    }

    void ForestTrainerBase::Sums::Print(std::ostream& os) const
    {
        os << "sumWeights = " << sumWeights << ", sumWeightedLabels = " << sumWeightedLabels;
    }
}
