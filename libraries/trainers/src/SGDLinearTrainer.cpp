////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDLinearTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SGDLinearTrainer.h"

namespace ell
{
namespace trainers
{
    void SGDLinearTrainerBase::Update(const data::AnyDataset& anyDataset) // TODO: move to cpp file
    {
        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

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
}
}