////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainer.h"

// utilites
#include "Exception.h"

namespace ell
{
namespace trainers
{
    //
    // Sums
    //
    void ForestTrainerBase::Sums::Increment(const data::WeightLabel& weightLabel)
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
        if (sumWeights == 0)
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
    ForestTrainerBase::NodeRanges::NodeRanges(const Range& totalRange)
        : _firstIndex(2)
    {
        _firstIndex[0] = totalRange.firstIndex;
        _firstIndex[1] = totalRange.firstIndex + totalRange.size;
    }

    ForestTrainerBase::Range ForestTrainerBase::NodeRanges::GetTotalRange() const
    {
        return Range{ _firstIndex.front(), _firstIndex.back() - _firstIndex.front() };
    }

    ForestTrainerBase::Range ForestTrainerBase::NodeRanges::GetChildRange(size_t childPosition) const
    {
        if (childPosition + 1 >= _firstIndex.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange);
        }

        return Range{ _firstIndex[childPosition], _firstIndex[childPosition + 1] - _firstIndex[childPosition] };
    }

    void ForestTrainerBase::NodeRanges::SplitChildRange(size_t childPosition, size_t size)
    {
        if (childPosition + 1 >= _firstIndex.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange);
        }

        auto childIterator = _firstIndex.begin() + childPosition;
        if (*childIterator + size > *(childIterator + 1))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "size too big");
        }

        _firstIndex.insert(childIterator + 1, *childIterator + size);
    }

    //
    // NodeStats
    //

    void ForestTrainerBase::NodeStats::SetChildSums(std::vector<Sums> childSums)
    {
        _childSums = childSums;
    }

    ForestTrainerBase::NodeStats::NodeStats(const Sums& totalSums)
        : _totalSums(totalSums), _childSums(2)
    {
    }

    const ForestTrainerBase::Sums& ForestTrainerBase::NodeStats::GetChildSums(size_t position) const
    {
        return _childSums[position];
    }

    ForestTrainerBase::TrainerMetadata::TrainerMetadata(const data::WeightLabel& metaData)
        : strong(metaData)
    {
    }

    //
    // debugging code
    //
    void ForestTrainerBase::TrainerMetadata::Print(std::ostream& os) const
    {
        os << "(" << strong.weight << ", " << strong.label << ", " << weak.weight << ", " << weak.label << ", " << currentOutput << ")";
    }

    void ForestTrainerBase::NodeStats::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "stats:\n";

        os << std::string((tabs + 1) * 4, ' ') << "sums:\t";
        _totalSums.Print(os);
        os << "\n";

        os << std::string((tabs + 1) * 4, ' ') << "sums0:\t";
        _childSums[0].Print(os);
        os << "\n";

        os << std::string((tabs + 1) * 4, ' ') << "sums1:\t";
        _childSums[1].Print(os);
        os << "\n";
    }
}
}
