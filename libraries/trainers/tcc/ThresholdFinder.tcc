////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ThresholdFinder.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThresholdFinder.h"

// stl
#include <algorithm>

namespace trainers
{
    template<typename ExampleIteratorType>
    std::vector<std::vector<ThresholdFinder::ValueCount>> ThresholdFinder::UniqueValues(ExampleIteratorType exampleIterator) const
    {
        std::vector<std::vector<ValueCount>> result;

        // invert and densify result
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();
            const auto& denseDataVector = example.GetDataVector();
            double weight = example.GetMetaData().weak.weight;

            if (result.size() < denseDataVector.Size())
            {
                result.resize(denseDataVector.Size());
            }

            for (size_t j = 0; j < denseDataVector.Size(); ++j)
            {
                result[j].push_back({ denseDataVector[j], weight });
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
}