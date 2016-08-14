////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ThresholdFinder.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThresholdFinder.h"

// stl
#include <algorithm>

namespace trainers
{
    std::vector<std::vector<ThresholdFinder::ValueCount>> ThresholdFinder::UniqueValues(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        std::vector<std::vector<ValueCount>> result;

        // invert and densify result
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            auto denseDataVector = example.GetDataVector().ToArray();

            if (result.size() < denseDataVector.size())
            {
                result.resize(denseDataVector.size());
            }

            for (size_t j = 0; j < denseDataVector.size(); ++j)
            {
                result[j].push_back({ denseDataVector[j], 1 });
            }

            exampleIterator.Next();
        }

        // sort and unique each feature
        for (size_t j = 0; j < result.size(); ++j)
        {
            auto newSize = SortReduceDuplicates(result[j].begin(), result[j].end());
            result[j].resize(newSize);
        }

        return result;
    }

    size_t ThresholdFinder::SortReduceDuplicates(std::vector<ValueCount>::iterator begin, const std::vector<ValueCount>::iterator end) const
    {
        // sort the values
        std::sort(begin, end);

        auto iter = begin;
        auto current = begin;

        // remove duplicates while incrementing counts
        while (++iter != end)
        {
            if (iter->value == current->value)
            {
                current->count += iter->count;
            }
            else
            {
                if (++current != iter)
                {
                    *current = std::move(*iter);
                }
            }
        }

        return current - begin + 1;
    }
}