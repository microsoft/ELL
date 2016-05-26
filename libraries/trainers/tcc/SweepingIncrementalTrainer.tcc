////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SweepingIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "RandomEngines.h"

namespace trainers
{
    template <typename PredictorType>
    SweepingIncrementalTrainer<PredictorType>::SweepingIncrementalTrainer(std::vector<std::unique_ptr<IIncrementalTrainer<PredictorType>>>&& incrementalTrainers, const MultiEpochIncrementalTrainerParameters& parameters, std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluators) :
        _incrementalTrainers(std::move(incrementalTrainers)), _parameters(parameters), _evaluators(std::move(evaluators)), _random(utilities::GetRandomEngine(parameters.dataPermutationRandomSeed))
    {}
    
    template <typename PredictorType>
    void SweepingIncrementalTrainer<PredictorType>::Update(dataset::GenericRowDataset::Iterator exampleIterator)
    {
        dataset::GenericRowDataset rowDataset(exampleIterator);

        // calculate epoch size
        uint64_t epochSize = _parameters.epochSize;
        if(epochSize == 0 || epochSize >  rowDataset.NumExamples())
        {
            epochSize = rowDataset.NumExamples();
        }

        for(int epoch = 0; epoch < _parameters.numEpochs; ++epoch)
        {
            // randomly permute the data
            rowDataset.RandomPermute(_random, epochSize);

            // update the incremental trainer
            //auto trainSetIterator = rowDataset.GetIterator(0, epochSize);
            //_incrementalTrainer->Update(trainSetIterator);

            //    _evaluator->Evaluate(*_incrementalTrainer->GetPredictor());
        }
    }

    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeSweepingIncrementalTrainer(std::vector<std::unique_ptr<IIncrementalTrainer<PredictorType>>>&& incrementalTrainers, const MultiEpochIncrementalTrainerParameters& parameters, std::vector<std::shared_ptr<evaluators::IEvaluator<PredictorType>>> evaluators)
    {
        return std::make_unique<SweepingIncrementalTrainer<PredictorType>>(std::move(incrementalTrainers), parameters, std::move(evaluators));
    }
}