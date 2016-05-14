////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochMetaTrainer.h (statefulTrainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"
#include "IStatefulTrainer.h"

// dataset
#include "RowDataset.h"

//stl
#include <utility>
#include <memory>

namespace trainers
{
    template <typename PredictorType>
    class MultiEpochMetaTrainer : public ITrainer<PredictorType>
    {
    public:
        MultiEpochMetaTrainer() = delete;
        MultiEpochMetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer) : _statefulTrainer(std::move(statefulTrainer)) {}
        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const {/*TODO: first do iterations of Update*/ return _statefulTrainer->Reset();}

    private: 
        std::unique_ptr<IStatefulTrainer<PredictorType>> _statefulTrainer;
    };

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochMetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer)
    {
        return MultiEpochMetaTrainer<PredictorType>(std::move(statefulTrainer));
    }
}