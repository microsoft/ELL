////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SweepingTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EvaluatingTrainer.h"
#include "ITrainer.h"

#include <data/include/Dataset.h>

#include <evaluators/include/Evaluator.h>

//stl
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace ell
{
namespace trainers
{
    /// <summary> A class that runs multiple internal trainers and chooses the best performing predictor. </summary>
    ///
    /// <typeparam name="PredictorType"> The type of predictor returned by this trainer. </typeparam>
    template <typename PredictorType>
    class SweepingTrainer : public ITrainer<PredictorType>
    {
    public:
        using EvaluatingTrainerType = EvaluatingTrainer<PredictorType>;
        using ExampleType = data::Example<typename PredictorType::DataVectorType, data::WeightLabel>;

        /// <summary> Constructs an instance of SweepingTrainer. </summary>
        ///
        /// <param name="evaluatingTrainers"> A vector of evaluating trainers. </param>
        SweepingTrainer(std::vector<EvaluatingTrainerType>&& evaluatingTrainers);

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        void Update() override;

        /// <summary> Gets a const reference to the current predictor. </summary>
        ///
        /// <returns> A const reference to the current predictor. </returns>
        const PredictorType& GetPredictor() const override;

    private:
        data::Dataset<ExampleType> _dataset;
        std::vector<EvaluatingTrainerType> _evaluatingTrainers;
    };

    /// <summary> Makes an incremental trainer that runs multiple internal trainers and chooses the best performing predictor. </summary>
    ///
    /// <typeparam name="PredictorType"> Type of the predictor returned by this trainer. </typeparam>
    /// <param name="evaluatingTrainers"> A vector of evaluating trainers. </param>
    ///
    /// <returns> A unique_ptr to a sweeping trainer. </returns>
    template <typename PredictorType>
    std::unique_ptr<ITrainer<PredictorType>> MakeSweepingTrainer(std::vector<EvaluatingTrainer<PredictorType>>&& evaluatingTrainers);
} // namespace trainers
} // namespace ell

#pragma region implementation

namespace ell
{
namespace trainers
{
    template <typename PredictorType>
    SweepingTrainer<PredictorType>::SweepingTrainer(std::vector<EvaluatingTrainerType>&& evaluatingTrainers) :
        _evaluatingTrainers(std::move(evaluatingTrainers))
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
} // namespace trainers
} // namespace ell

#pragma endregion implementation
