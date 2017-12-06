////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SweepingTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace trainers
{
    template <typename PredictorType>
    SweepingTrainer<PredictorType>::SweepingTrainer(std::vector<EvaluatingTrainerType>&& evaluatingTrainers)
        : _evaluatingTrainers(std::move(evaluatingTrainers))
    {
        assert(_evaluatingTrainers.size() > 0);
    }

    template <typename PredictorType>
    void SweepingTrainer<PredictorType>::SetDataset(const data::AnyDataset& anyDataset)
    {
        _dataset = data::Dataset<ExampleType>(anyDataset);
    }

    template <typename PredictorType>
    void SweepingTrainer<PredictorType>::Update()
    {
        for (size_t i = 0; i < _evaluatingTrainers.size(); ++i)
        {
            _evaluatingTrainers[i].Update();
        }
    }

    template <typename PredictorType>
    const PredictorType& SweepingTrainer<PredictorType>::GetPredictor() const
    {
        double bestGoodness = _evaluatingTrainers[0].GetEvaluator()->GetGoodness();
        size_t bestIndex = 0;
        for (size_t i = 1; i < _evaluatingTrainers.size(); ++i)
        {
            double goodness = _evaluatingTrainers[i].GetEvaluator()->GetGoodness();
            if (goodness > bestGoodness)
            {
                bestGoodness = goodness;
                bestIndex = i;
            }
        }

        return _evaluatingTrainers[bestIndex].GetPredictor();
    }

    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSweepingTrainer(std::vector<EvaluatingTrainer<PredictorType>>&& evaluatingTrainers)
    {
        return std::make_unique<SweepingTrainer<PredictorType>>(std::move(evaluatingTrainers));
    }
}
}
