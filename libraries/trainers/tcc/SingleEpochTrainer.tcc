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
    SingleEpochTrainer<PredictorType>::SingleEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& learner) : 
        _incrementalTrainer(std::move(learner)) 
    {}
    
    template <typename PredictorType>
    PredictorType SingleEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        _incrementalTrainer->Update(exampleIterator);
        return _incrementalTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<SingleEpochTrainer<PredictorType>> MakeSingleEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& learner)
    {
        return std::make_unique<SingleEpochTrainer<PredictorType>>(std::move(learner));
    }
}