////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "RandomEngines.h"

namespace trainers
{
    template <typename PredictorType>
    MultiEpochTrainer<PredictorType>::MultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, const MultiEpochTrainerParameters& parameters) :
        _incrementalTrainer(std::move(incrementalTrainer)), _parameters(parameters), _random(utilities::GetRandomEngine(parameters.dataPermutationRandomSeed))
    {}
    
    template <typename PredictorType>
    PredictorType MultiEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
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

            // iterate over the entire permuted dataset
            auto trainSetIterator = rowDataset.GetIterator(0, epochSize);
            _incrementalTrainer->Update(trainSetIterator);


        //    // Evaluate training error
        //    auto evaluationIterator = rowDataset.GetIterator();
        //    evaluator->Evaluate(evaluationIterator, trainer->GetPredictor());
        }

        return _incrementalTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer, const MultiEpochTrainerParameters& parameters)
    {
        return std::make_unique<MultiEpochTrainer<PredictorType>>(std::move(incrementalTrainer), parameters);
    }
}