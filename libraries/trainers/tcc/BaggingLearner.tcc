#include "..\include\BaggingLearner.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingLearner.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{

    template<typename BasePredictorType>
    BaggingLearner<BasePredictorType>::BaggingLearner(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer)
    {
        // TODO
    }

    template<typename BasePredictorType>
    void BaggingLearner<BasePredictorType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        // TODO
    }

    template<typename BasePredictorType>
    predictors::EnsemblePredictor<BasePredictorType> BaggingLearner<BasePredictorType>::Reset()
    {
        // TODO
        return predictors::EnsemblePredictor<BasePredictorType>();
    }

    template<typename BasePredictorType>
    const predictors::EnsemblePredictor<BasePredictorType>& BaggingLearner<BasePredictorType>::GetPredictor() const
    {
        return _ensemble;
    }

    template<typename BasePredictorType>
    std::unique_ptr<ILearner<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingLearner(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer)
    {
        // TODO
        return std::unique_ptr<ILearner<predictors::EnsemblePredictor<BasePredictorType>>>();
    }

    template<typename BasePredictorType>
    std::unique_ptr<ITrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer)
    {
        // TODO
        return std::unique_ptr<ITrainer<predictors::EnsemblePredictor<BasePredictorType>>>();
    }
}