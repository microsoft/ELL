////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "RandomEngines.h"

namespace emll
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
    void MultiEpochIncrementalTrainer<PredictorType>::Update(data::Dataset dataset)
    {
        auto rowDataset = data::MakeRowDataset(dataset.GetIterator<data::AutoSupervisedExample>()); // TODO match internal trainer example type

        // calculate epoch size
        uint64_t epochSize = _parameters.epochSize;
        if (epochSize == 0 || epochSize > rowDataset.NumExamples())
        {
            epochSize = rowDataset.NumExamples();
        }

        for (int epoch = 0; epoch < _parameters.numEpochs; ++epoch)
        {
            // randomly permute the data
            rowDataset.RandomPermute(_random, epochSize);

            // update the incremental trainer
            _internalTrainer->Update(rowDataset.GetDataset(0, epochSize));
        }
    }

    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeMultiEpochIncrementalTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& internalTrainer, const MultiEpochIncrementalTrainerParameters& parameters)
    {
        return std::make_unique<MultiEpochIncrementalTrainer<PredictorType>>(std::move(internalTrainer), parameters);
    }
}
}
