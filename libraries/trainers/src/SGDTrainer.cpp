////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SGDTrainer.h"

namespace ell
{
namespace trainers
{

    void SGDTrainerBase::SetDataset(const data::AnyDataset& anyDataset)
    {
        _dataset = data::Dataset<data::AutoSupervisedExample>(anyDataset);
    }

    void SGDTrainerBase::Update()
    {
        // permute the data
        _dataset.RandomPermute(_random);

        // get example iterator
        auto exampleIterator = _dataset.GetExampleReferenceIterator();

        // first iteration handled separately
        if (_firstIteration && exampleIterator.IsValid())
        {
            const auto& example = exampleIterator.Get();

            const auto& x = example.GetDataVector();
            double y = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;

            DoFirstStep(x, y, weight);

            exampleIterator.Next();
            _firstIteration = false;
        }

        while (exampleIterator.IsValid())
        {
            // get the Next example
            const auto& example = exampleIterator.Get();

            const auto& x = example.GetDataVector();
            double y = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;

            DoNextStep(x, y, weight);

            exampleIterator.Next();
        }
    }

    SGDTrainerBase::SGDTrainerBase(std::string randomSeedString)
    {
        std::seed_seq seed(randomSeedString.begin(), randomSeedString.end());
        _random = std::default_random_engine(seed);
    }
}
}