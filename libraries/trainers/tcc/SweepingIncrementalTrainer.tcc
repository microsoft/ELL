////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SweepingIncrementalTrainer.tcc (trainers)
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
    SweepingIncrementalTrainer<PredictorType>::SweepingIncrementalTrainer(std::vector<EvaluatingTrainerType>&& evaluatingTrainers, const MultiEpochIncrementalTrainerParameters& parameters)
        : _evaluatingTrainers(std::move(evaluatingTrainers)), _parameters(parameters), _random(utilities::GetRandomEngine(parameters.dataPermutationRandomSeed))
    {
        assert(_evaluatingTrainers.size() > 0);
    }

    template <typename PredictorType>
    void SweepingIncrementalTrainer<PredictorType>::Update(data::Dataset dataset)
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

            for (int i = 0; i < _evaluatingTrainers.size(); ++i)
            {
                // update the incremental trainer
                _evaluatingTrainers[i].Update(rowDataset.GetDataset(0, epochSize));
            }
        }
    }

    template <typename PredictorType>
    const std::shared_ptr<const PredictorType> SweepingIncrementalTrainer<PredictorType>::GetPredictor() const
    {
        double bestGoodness = _evaluatingTrainers[0].GetEvaluator()->GetGoodness();
        int best = 0;
        for (int i = 1; i < _evaluatingTrainers.size(); ++i)
        {
            double goodness = _evaluatingTrainers[i].GetEvaluator()->GetGoodness();
            if (goodness > bestGoodness)
            {
                bestGoodness = goodness;
                best = i;
            }
        }

        return _evaluatingTrainers[best].GetPredictor();
    }

    template <typename PredictorType>
    std::unique_ptr<IIncrementalTrainer<PredictorType>> MakeSweepingIncrementalTrainer(std::vector<EvaluatingIncrementalTrainer<PredictorType>>&& evaluatingTrainers, const MultiEpochIncrementalTrainerParameters& parameters)
    {
        return std::make_unique<SweepingIncrementalTrainer<PredictorType>>(std::move(evaluatingTrainers), parameters);
    }
}
}
