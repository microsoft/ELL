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
    BaggingIncrementalTrainer<BasePredictorType>::BaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer, const BaggingIncrementalTrainerParameters& parameters) : _parameters(parameters)
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
    std::unique_ptr<IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer, , const BaggingIncrementalTrainerParameters& parameters)
    {
        return std::make_unique<BaggingIncrementalTrainer<BasePredictorType>>(std::move(trainer), parameters);
    }
}