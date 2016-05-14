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
    MultiEpochTrainer<PredictorType>::MultiEpochTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer, const MultiEpochTrainerParameters& parameters) :
        _statefulTrainer(std::move(statefulTrainer)), _parameters(parameters)
    {}
    
    template <typename PredictorType>
    PredictorType MultiEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        // TODO: constuct shallow copy of dataset
        
        // TODO: call the following function _parameters.numEpochs times
        _statefulTrainer->Update(exampleIterator);

        return _statefulTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer, const MultiEpochTrainerParameters& parameters)
    {
        return std::make_unique<MultiEpochTrainer<PredictorType>>(std::move(statefulTrainer), parameters);
    }
}