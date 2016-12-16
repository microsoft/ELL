////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultiEpochIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "RandomEngines.h"

namespace ell
{
namespace trainers
{
    template <typename PredictorType>
    MultiEpochIncrementalTrainer<PredictorType>::MultiEpochIncrementalTrainer(std::unique_ptr<InternalTrainerType>&& internalTrainer, const MultiEpochIncrementalTrainerParameters& parameters)
        : _internalTrainer(std::move(internalTrainer)), _parameters(parameters), _random(utilities::GetRandomEngine(parameters.dataPermutationRandomSeed))
    {
        assert(_internalTrainer != nullptr);
    }

    template <typename PredictorType>
    void MultiEpochIncrementalTrainer<PredictorType>::Update(const data::AnyDataset& anyDataset)
    {
        using ExampleType = data::Example<typename PredictorType::DataVectorType, data::WeightLabel>;
        auto dataset = data::Dataset<ExampleType>(anyDataset);

        // calculate epoch size
        size_t epochSize = _parameters.epochSize;
        if (epochSize == 0 || epochSize > dataset.NumExamples())
        {
            epochSize = dataset.NumExamples();
        }

        for (int epoch = 0; epoch < _parameters.numEpochs; ++epoch)
        {
            // randomly permute the data
            dataset.RandomPermute(_random, epochSize);

            // update the incremental trainer
            _internalTrainer->Update(dataset.GetAnyDataset(0, epochSize));
        }
    }

    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeMultiEpochIncrementalTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer, const MultiEpochIncrementalTrainerParameters& parameters)
    {
        return std::make_unique<MultiEpochIncrementalTrainer<PredictorType>>(std::move(internalTrainer), parameters);
    }
}
}
