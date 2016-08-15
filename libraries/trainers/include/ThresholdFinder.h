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

// stl
#include <vector>

namespace trainers
{
    class ThresholdFinder
    {
    public:
        struct ValueCount
        {
            double value;
            double weight;

            operator double() { return value; }
        };

        /// <summary> Gets a vector of sorted unique values from each feature, with counts. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator. </param>
        ///
        /// <returns> For each feature, a vector of sorted unique values with counts. </returns>
        std::vector<std::vector<ValueCount>> UniqueValues(dataset::GenericRowDataset::Iterator exampleIterator) const;

    private:
       size_t SortReduceDuplicates(std::vector<ValueCount>::iterator begin, std::vector<ValueCount>::iterator end) const;
    };
}