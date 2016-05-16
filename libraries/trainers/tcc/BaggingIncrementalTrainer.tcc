#include "..\include\BaggingIncrementalTrainer.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{

    template<typename BasePredictorType>
    BaggingIncrementalTrainer<BasePredictorType>::BaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer)
    {
        // TODO
    }

    template<typename BasePredictorType>
    void BaggingIncrementalTrainer<BasePredictorType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // TODO
    }

    template<typename BasePredictorType>
    predictors::EnsemblePredictor<BasePredictorType> BaggingIncrementalTrainer<BasePredictorType>::Reset()
    {
        // TODO
        return predictors::EnsemblePredictor<BasePredictorType>();
    }

    template<typename BasePredictorType>
    const predictors::EnsemblePredictor<BasePredictorType>& BaggingIncrementalTrainer<BasePredictorType>::GetPredictor() const
    {
        return _ensemble;
    }

    template<typename BasePredictorType>
    std::unique_ptr<BaggingIncrementalTrainer<BasePredictorType>> MakeBaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer)
    {
        // TODO
        return std::unique_ptr<BaggingIncrementalTrainer<BasePredictorType>>();
    }

    template<typename BasePredictorType>
    std::unique_ptr<BaggingIncrementalTrainer<BasePredictorType>> MakeBaggingTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer)
    {
        // TODO
        return std::unique_ptr<BaggingIncrementalTrainer<BasePredictorType>>();
    }
}