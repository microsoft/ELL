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

    void ForestTrainerBase::Sums::Increment(const ExampleMetaData& metaData)
    {
        sumWeights += metaData.weakWeight;
        sumWeightedLabels += metaData.weakWeight * metaData.weakLabel;
    }

    ForestTrainerBase::Sums ForestTrainerBase::Sums::operator-(const ForestTrainerBase::Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    //
    // debugging code
    // 

    void ForestTrainerBase::Sums::Print(std::ostream& os) const
    {
        os << "sumWeights = " << sumWeights << ", sumWeightedLabels = " << sumWeightedLabels;
    }

    void ForestTrainerBase::NodeStats::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "stats:\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums:\t";
        sums.Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums0:\t";
        sums0.Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums1:\t";
        sums1.Print(os);
        os << "\n";
    }
}