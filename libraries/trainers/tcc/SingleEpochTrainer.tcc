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
    SingleEpochTrainer<PredictorType>::SingleEpochTrainer(std::unique_ptr<ILearner<PredictorType>>&& learner) : 
        _learner(std::move(learner)) 
    {}
    
    template <typename PredictorType>
    PredictorType SingleEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        _learner->Update(exampleIterator);
        return _learner->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSingleEpochTrainer(std::unique_ptr<ILearner<PredictorType>>&& learner)
    {
        return std::make_unique<SingleEpochTrainer<PredictorType>>(std::move(learner));
    }
}