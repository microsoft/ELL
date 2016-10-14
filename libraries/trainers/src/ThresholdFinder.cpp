////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ThresholdFinder.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThresholdFinder.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace emll
{
namespace trainers
{
    size_t ThresholdFinder::SortReduceDeepCopyAss(std::vector<ValueWeight>::iterator begin, const std::vector<ValueWeight>::iterator end) const
    {
        // sort the values
        std::sort(begin, end, std::less<double>());

        auto iter = begin;
        auto current = begin;

        // remove DeepCopyAss while incrementing counts
        while (++iter != end)
        {
            if (iter->value == current->value)
            {
                current->weight += iter->weight;
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
}
