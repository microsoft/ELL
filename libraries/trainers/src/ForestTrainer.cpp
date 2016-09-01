////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainer.h"

// utilites
#include "Exception.h"

namespace trainers
{
    //
    // Sums
    //
    void ForestTrainerBase::Sums::Increment(const dataset::WeightLabel& weightLabel)
    {
        sumWeights += weightLabel.weight;
        sumWeightedLabels += weightLabel.weight * weightLabel.label;
    }

    typename ForestTrainerBase::Sums ForestTrainerBase::Sums::operator-(const Sums& other) const
    {
        Sums difference;
        difference.sumWeights = sumWeights - other.sumWeights;
        difference.sumWeightedLabels = sumWeightedLabels - other.sumWeightedLabels;
        return difference;
    }

    double ForestTrainerBase::Sums::GetMeanLabel() const
    {
        if(sumWeights == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "Can't compute mean because sum of weights equals zero");
        }
        return sumWeightedLabels / sumWeights;
    }

    void ForestTrainerBase::Sums::Print(std::ostream& os) const
    {
        os << "sumWeights = " << sumWeights << ", sumWeightedLabels = " << sumWeightedLabels;
    }

    //
    // NodeRanges
    //
    ForestTrainerBase::NodeRanges::NodeRanges(const Range& totalRange) : _total(totalRange)
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

    //
    // NodeStats
    //

    void ForestTrainerBase::NodeStats::SetChildSums(std::vector<Sums> childSums) 
    { 
        _childSums = childSums; 
    } 

    ForestTrainerBase::NodeStats::NodeStats(const Sums& totalSums) : _totalSums(totalSums), _childSums(2)
    {}

    const ForestTrainerBase::Sums& ForestTrainerBase::NodeStats::GetChildSums(size_t position) const
    {
        return _childSums[position];
    }

    //
    // debugging code
    //
    void ForestTrainerBase::ExampleMetadata::Print(std::ostream & os) const
    {
        os << "(" << strong.weight << ", " << strong.label << ", " << weak.weight << ", " << weak.label << ", " << currentOutput << ")";
    }

    void ForestTrainerBase::NodeStats::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "stats:\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums:\t";
        _totalSums.Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums0:\t";
        _childSums[0].Print(os);
        os << "\n";

        os << std::string((tabs+1) * 4, ' ') <<  "sums1:\t";
        _childSums[1].Print(os);
        os << "\n";
    }
}
