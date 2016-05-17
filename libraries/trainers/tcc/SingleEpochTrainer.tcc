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
    SingleEpochTrainer<PredictorType>::SingleEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer) : 
        _incrementalTrainer(std::move(incrementalTrainer)) 
    {}
    
    template <typename PredictorType>
    PredictorType SingleEpochTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        _incrementalTrainer->Update(exampleIterator);
        return _incrementalTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSingleEpochTrainer(std::unique_ptr<IIncrementalTrainer<PredictorType>>&& incrementalTrainer)
    {
        return std::make_unique<SingleEpochTrainer<PredictorType>>(std::move(incrementalTrainer));
    }
}