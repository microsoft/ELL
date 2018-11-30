////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ThresholdFinder.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <data/include/Dataset.h>

#include <predictors/include/SingleElementThresholdPredictor.h>

#include <algorithm>
#include <vector>

namespace ell
{
namespace trainers
{
    /// <summary> Base class for threshold predictor finders. </summary>
    class ThresholdFinder
    {
    protected:
        using DataVectorType = predictors::SingleElementThresholdPredictor::DataVectorType;

        struct ValueWeight
        {
            double value;
            double weight;

            operator double() { return value; }
        };

        // the result of a call to UniqueValues
        struct UniqueValuesResult
        {
            std::vector<std::vector<ThresholdFinder::ValueWeight>> weightedValues;
            double totalWeight;
        };

        // Gets a vector of sorted unique values from each feature, with counts
        template <typename ExampleIteratorType>
        UniqueValuesResult UniqueValues(ExampleIteratorType exampleIterator) const;

    private:
        size_t SortReduceCopy(std::vector<ValueWeight>::iterator begin, const std::vector<ValueWeight>::iterator end) const;
    };

    /// <summary> A threshold finder that finds all possible thresholds. </summary>
    class ExhaustiveThresholdFinder : public ThresholdFinder
    {
    public:
        /// <summary> Returns a vector of SingleElementThresholdPredictor. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of example iterator. </typeparam>
        /// <param name="exampleIterator"> The example iterator. </param>
        ///
        /// <returns> The thresholds. </returns>
        template <typename ExampleIteratorType>
        std::vector<predictors::SingleElementThresholdPredictor> GetThresholds(ExampleIteratorType exampleIterator) const;
    };
} // namespace trainers
} // namespace ell

#pragma region implementation

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
} // namespace trainers
} // namespace ell

#pragma endregion implementation
