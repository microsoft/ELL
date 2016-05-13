////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochMetaTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace trainers
{
    template <typename TrainerType>
    class MultiEpochMetaTrainer
    {
        auto Train(ISortingTreeTrainer::ExampleIteratorType exampleIterator) const -> decltype(TrainerType::Reset());
    };
}