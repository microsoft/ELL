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

    void ForestTrainerBase::ExampleMetaData::Print(std::ostream & os) const
    {
        os << "(" << weight << ", " << label << ", " << weakWeight << ", " << weakLabel << ")";
    }

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

    void ForestTrainerBase::NodeStats::SetChildSums(std::vector<ForestTrainerBase::Sums> childSums) 
    { 
        _childSums = childSums; 
    } 

    ForestTrainerBase::NodeStats::NodeStats(const ForestTrainerBase::Sums& totalSums) : _totalSums(totalSums), _childSums(2)
    {}

    const ForestTrainerBase::Sums& ForestTrainerBase::NodeStats::GetChildSums(size_t position) const
    {
        return _childSums[position];
    }

    ForestTrainerBase::Sums ForestTrainerBase::LoadData(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        Sums sums;

        // create DenseRowDataset
        while (exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();

            ExampleMetaData metaData = example.GetMetaData();

            // set weak label/weight to equal strong label/weight
            metaData.weakLabel = metaData.label;
            metaData.weakWeight = metaData.weight;
            sums.Increment(metaData);

            // TODO this code breaks encapsulation - give ForestTrainer a ctor that takes an IDataVector
            auto denseDataVector = std::make_unique<dataset::DoubleDataVector>(example.GetDataVector().ToArray());
            auto forestTrainerExample = ForestTrainerExample(std::move(denseDataVector), metaData);
            _dataset.AddExample(std::move(forestTrainerExample));

            exampleIterator.Next();
        }

        if(sums.sumWeights == 0.0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badData, "sum of weights in data is zero");
        }

        return sums;
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