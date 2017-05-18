////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ThresholdFinder.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThresholdFinder.h"

// stl
#include <algorithm>

namespace ell
{
namespace trainers
{
    template <typename ExampleIteratorType>
    ThresholdFinder::UniqueValuesResult ThresholdFinder::UniqueValues(ExampleIteratorType exampleIterator) const
    {
        std::vector<std::vector<ValueWeight>> result;
        double totalWeight = 0.0;

        // invert and densify result
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            const auto& denseDataVector = example.GetDataVector();
            double weight = example.GetMetadata().weak.weight;

            totalWeight += weight;

            if (result.size() < denseDataVector.PrefixLength())
            {
                result.resize(denseDataVector.PrefixLength());
            }

            for (size_t j = 0; j < denseDataVector.PrefixLength(); ++j)
            {
                result[j].push_back({ denseDataVector[j], weight });
            }

            exampleIterator.Next();
        }

        // sort and unique each feature
        for (size_t j = 0; j < result.size(); ++j)
        {
            auto newSize = SortReduceCopy(result[j].begin(), result[j].end());
            result[j].resize(newSize);
        }

        return { result, totalWeight };
    }

    template <typename ExampleIteratorType>
    std::vector<predictors::SingleElementThresholdPredictor> trainers::ExhaustiveThresholdFinder::GetThresholds(ExampleIteratorType exampleIterator) const
    {
        auto uniqueValuesResult = UniqueValues(exampleIterator);
        std::vector<predictors::SingleElementThresholdPredictor> thresholdPredictors;

        for (size_t j = 0; j < uniqueValuesResult.weightedValues.size(); ++j)
        {
            const auto& featureValues = uniqueValuesResult.weightedValues[j];
            for (size_t i = 0; i < featureValues.size() - 1; ++i)
            {
                thresholdPredictors.push_back({ j, 0.5 * (featureValues[i].value + featureValues[i + 1].value) });
            }
        }

        return thresholdPredictors;
    }
}
}
