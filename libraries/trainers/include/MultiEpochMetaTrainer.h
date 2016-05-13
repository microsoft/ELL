////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochMetaTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

//stl
#include <utility>
#include <memory>
namespace trainers
{
    template <typename TrainerType>
    class MultiEpochMetaTrainer
    {
    public:
        MultiEpochMetaTrainer() = delete;

        MultiEpochMetaTrainer(std::unique_ptr<TrainerType>&& trainer) : _trainer(std::move(trainer)) {}

        auto Train(ISortingTreeTrainer::ExampleIteratorType exampleIterator) const -> decltype(std::declval<TrainerType>().Reset()) {return _trainer->Reset();}

    private: 
        std::unique_ptr<TrainerType> _trainer;
    };
}