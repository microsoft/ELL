////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochMetaTrainer.h (updaters)
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
    using ExampleIteratorType = dataset::GenericRowDataset::Iterator;

    template <typename PredictorType>
    class ITrainer
    {
    public:

        virtual ~ITrainer() = default;

        virtual PredictorType Train(ExampleIteratorType exampleIterator) const = 0;
    };

    template <typename PredictorType>
    class IUpdater
    {
    public:

        virtual ~IUpdater() = default;

        virtual void Update(ExampleIteratorType exampleIterator) = 0;
        virtual const PredictorType& GetPredictor() const = 0;
        virtual PredictorType Reset() = 0;
    };

    template <typename PredictorType>
    class MultiEpochMetaTrainer : public ITrainer<PredictorType>
    {
    public:
        MultiEpochMetaTrainer() = delete;
        MultiEpochMetaTrainer(std::unique_ptr<IUpdater<PredictorType>>&& updater) : _updater(std::move(updater)) {}
        virtual PredictorType Train(ExampleIteratorType exampleIterator) const {/*TODO: first do iterations of Update*/ return _updater->Reset();}

    private: 
        std::unique_ptr<IUpdater<PredictorType>> _updater;
    };

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochMetaTrainer(std::unique_ptr<IUpdater<PredictorType>>&& updater)
    {
        return MultiEpochMetaTrainer<PredictorType>(std::move(updater));
    }
}