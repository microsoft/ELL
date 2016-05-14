////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleEpochTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{
    template <typename PredictorType>
    SingleEpochTrainer<PredictorType>::SingleEpochTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer) : 
        _statefulTrainer(std::move(statefulTrainer)) 
    {}
    
    template <typename PredictorType>
    PredictorType SingleEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        _statefulTrainer->Update(exampleIterator);
        return _statefulTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSingleEpochTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer)
    {
        return std::make_unique<SingleEpochTrainer<PredictorType>>(std::move(statefulTrainer));
    }
}