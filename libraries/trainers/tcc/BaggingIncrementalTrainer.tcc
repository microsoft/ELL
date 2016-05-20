////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "RandomEngines.h"

namespace trainers
{
    template<typename BasePredictorType>
    BaggingIncrementalTrainer<BasePredictorType>::BaggingIncrementalTrainer(std::unique_ptr<ITrainer<BasePredictorType>>&& trainer, const BaggingIncrementalTrainerParameters& baggingParameters, std::shared_ptr<evaluators::IEvaluator<PredictorType>> evaluator) :
        _trainer(std::move(trainer)), _baggingParameters(baggingParameters), _ensemble(std::make_shared<PredictorType>()), _evaluator(std::move(evaluator)), _random(utilities::GetRandomEngine(baggingParameters.dataPermutationRandomSeed))
    {}

    template<typename BasePredictorType>
    void BaggingIncrementalTrainer<BasePredictorType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        dataset::GenericRowDataset rowDataset(exampleIterator);

        // calculate epoch size
        uint64_t bagSize = _baggingParameters.bagSize;
        if(bagSize == 0 || bagSize >  rowDataset.NumExamples())
        {
            bagSize = rowDataset.NumExamples();
        }

        // calculate base predictor weight
        double weight = 1.0 / _baggingParameters.numIterations;

        // iterate
        for(int i = 0; i < _baggingParameters.numIterations; ++i)
        {
            // get random bag of data
            rowDataset.RandomPermute(_random, bagSize);
            auto trainSetIterator = rowDataset.GetIterator(0, bagSize);

            // append weak predictor to the ensemble
            _ensemble->AppendPredictor(_trainer->Train(trainSetIterator), weight);

            // evaluate
            if (_evaluator != nullptr)
            {
                _evaluator->Evaluate(*_ensemble);
            }
        }
    }

    template<typename BasePredictorType>
    std::unique_ptr<IIncrementalTrainer<predictors::EnsemblePredictor<BasePredictorType>>> MakeBaggingIncrementalTrainer(
        std::unique_ptr<ITrainer<BasePredictorType>>&& trainer,
        const BaggingIncrementalTrainerParameters& baggingParameters, 
        std::shared_ptr<evaluators::IEvaluator<predictors::EnsemblePredictor<BasePredictorType>>> evaluator)
    {
        return std::make_unique<BaggingIncrementalTrainer<BasePredictorType>>(std::move(trainer), baggingParameters, std::move(evaluator));
    }
}