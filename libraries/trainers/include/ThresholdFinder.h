////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ThresholdFinder.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// data
#include "Dataset.h"

// predictor
#include "SingleElementThresholdPredictor.h"

// stl
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
}
}

#include "../tcc/ThresholdFinder.tcc"