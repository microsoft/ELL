////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MetaTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace trainers
{
    template <typename PredictorType>
    MetaTrainer<PredictorType>::MetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer) : 
        _statefulTrainer(std::move(statefulTrainer)) 
    {}
    
    template <typename PredictorType>
    PredictorType MetaTrainer<PredictorType>::Train(dataset::GenericRowDataset::Iterator exampleIterator) const
    {
        _statefulTrainer->Update(exampleIterator);
        return _statefulTrainer->Reset();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeMetaTrainer(std::unique_ptr<IStatefulTrainer<PredictorType>>&& statefulTrainer)
    {
        return std::make_unique<MetaTrainer<PredictorType>>(std::move(statefulTrainer));
    }
}