////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ThresholdFinder.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

// predictor
#include "SingleElementThresholdPredictor.h"

// stl
#include <vector>

namespace trainers
{
    class ThresholdFinder
    {
    public:
        struct ValueWeight
        {
            double value;
            double weight;

            operator double() { return value; }
        };

    protected:

        struct UniqueValuesResult
        {
            std::vector<std::vector<ThresholdFinder::ValueWeight>> weightedValues;
            double totalWeight;
        };

        // Gets a vector of sorted unique values from each feature, with counts
        template<typename ExampleIteratorType>
        UniqueValuesResult UniqueValues(ExampleIteratorType exampleIterator) const;

    private:
        size_t SortReduceDuplicates(std::vector<ValueWeight>::iterator begin, const std::vector<ValueWeight>::iterator end) const;
    };

    class ExhaustiveThresholdFinder : public ThresholdFinder
    {
    public:
        template<typename ExampleIteratorType>
        std::vector<predictors::SingleElementThresholdPredictor> GetThresholds(ExampleIteratorType exampleIterator) const;
    };

}

#include "../tcc/ThresholdFinder.tcc"