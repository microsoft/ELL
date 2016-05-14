////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochMetaTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{
    template <typename PredictorType>
    MultiEpochMetaTrainer<PredictorType>::MultiEpochMetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer, const MultiEpochMetaTrainerParameters& parameters) :
        _statefulTrainer(std::move(statefulTrainer)), _parameters(parameters)
    {}
    
    template <typename PredictorType>
    PredictorType MultiEpochMetaTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        // TODO: constuct shallow copy of dataset
        
        // TODO: call the following function _parameters.numEpochs times
        _statefulTrainer->Update(exampleIterator);

        return _statefulTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMultiEpochMetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer, const MultiEpochMetaTrainerParameters& parameters)
    {
        return std::make_unique<MultiEpochMetaTrainer<PredictorType>>(std::move(statefulTrainer), parameters);
    }
}