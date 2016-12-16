////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AUCAggregator.cpp (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AUCAggregator.h"

// stl
#include <algorithm>

namespace ell
{
namespace evaluators
{
    void AUCAggregator::Update(double prediction, double label, double weight)
    {
        _aggregates.push_back(Aggregate{ prediction, label, weight });
    }

    std::vector<double> AUCAggregator::GetResult() const
    {
        // sort aggregates by prediction
        std::sort(_aggregates.begin(), _aggregates.end());

        // collect statistics
        double sumPositiveWeights = 0.0;
        double sumNegativeWeights = 0.0;
        double sumOrderedWeights = 0.0;

        for (size_t i = 0; i < _aggregates.size(); ++i)
        {
            double weight = _aggregates[i].weight;

            if (_aggregates[i].label <= 0)
            {
                sumNegativeWeights += weight;
            }
            else
            {
                sumPositiveWeights += weight;
                sumOrderedWeights += sumNegativeWeights * weight;
            }
        }

        // calcluate the AUC
        double auc = 0.0;
        if (sumPositiveWeights > 0 && sumNegativeWeights > 0)
        {
            auc = sumOrderedWeights / sumPositiveWeights / sumNegativeWeights;
        }

        return { auc };
    }

    void AUCAggregator::Reset()
    {
        _aggregates.resize(0);
    }

    bool AUCAggregator::Aggregate::operator<(const Aggregate& other) const
    {
        // order by prediction (ascending) and then by label (descending) - this will produce the most pessimistic AUC
        if (prediction < other.prediction) return true;
        if (prediction > other.prediction) return false;
        return label > other.label;
    }

    std::vector<std::string> AUCAggregator::GetValueNames() const
    {
        return { "AUC" };
    }
}
}
