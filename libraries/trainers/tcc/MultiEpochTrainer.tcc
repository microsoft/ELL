////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{
    template <typename PredictorType>
    MultiEpochTrainer<PredictorType>::MultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& learner, const MultiEpochTrainerParameters& parameters) :
        _learner(std::move(learner)), _parameters(parameters)
    {}
    
    template <typename PredictorType>
    PredictorType MultiEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        // TODO: constuct shallow copy of dataset
        
        // TODO: call the following function _parameters.numEpochs times
        _learner->Update(exampleIterator);

        return _learner->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<MultiEpochTrainer<PredictorType>> MakeMultiEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& learner, const MultiEpochTrainerParameters& parameters)
    {
        return std::make_unique<MultiEpochTrainer<PredictorType>>(std::move(learner), parameters);
    }
}