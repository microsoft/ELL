////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainer.h"

namespace trainers
{
    ForestTrainerBase::NodeRanges::NodeRanges(const ForestTrainerBase::Range& totalRange) : _total(totalRange)
    {}

    ForestTrainerBase::Range ForestTrainerBase::NodeRanges::GetChildRange(size_t childPosition) const
    {
        if (childPosition == 0)
        {
            return Range{ _total.firstIndex, _size0 };
        }
        else
        {
            return Range{ _total.firstIndex+_size0, _total.size-_size0 };
        }
    }

    void ForestTrainerBase::NodeRanges::SetSize0(size_t value)
    {
        _size0 = value;
    }

    ForestTrainerBase::ExampleMetaData::ExampleMetaData(const dataset::WeightLabel& weightLabel) : dataset::WeightLabel(weightLabel)
    {} 
}