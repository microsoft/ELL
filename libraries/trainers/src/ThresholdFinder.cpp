////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ThresholdFinder.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThresholdFinder.h"

namespace trainers
{
    size_t ThresholdFinder::SortReduceDuplicates(std::vector<ValueWeight>::iterator begin, const std::vector<ValueWeight>::iterator end) const
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