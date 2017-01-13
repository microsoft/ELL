////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDIncrementalTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cassert>
#include <cmath>

// data
#include "Dataset.h"

namespace ell
{
namespace trainers
{
    template <typename LossFunctionType>
    SGDIncrementalTrainer<LossFunctionType>::SGDIncrementalTrainer(size_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters), _total_iterations(1), _lastPredictor(dim), _averagedPredictor(std::make_shared<PredictorType>(dim)) // iterations start from 1 to prevent divide-by-zero
    {
    }

    template <typename LossFunctionType>
    void SGDIncrementalTrainer<LossFunctionType>::Update(const data::AnyDataset& anyDataset)
    {
        UpdateSparse(anyDataset.GetExampleIterator<data::AutoSupervisedExample>(), anyDataset.NumExamples());
    }

    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDIncrementalTrainer(size_t dim, const LossFunctionType& lossFunction, const SGDIncrementalTrainerParameters& parameters)
    {
        return std::make_unique<SGDIncrementalTrainer<LossFunctionType>>(dim, lossFunction, parameters);
    }

    template <typename LossFunctionType>
    void SGDIncrementalTrainer<LossFunctionType>::UpdateSparse(data::ExampleIterator<data::AutoSupervisedExample> exampleIterator, size_t numExamples)
    {
        // get references to the vector and biases
        auto& vLast = _lastPredictor.GetWeights();
        auto& vAvg = _averagedPredictor->GetWeights();

        double& bLast = _lastPredictor.GetBias();
        double& bAvg = _averagedPredictor->GetBias();

        // define some constants
        const double T_prev = double(_total_iterations);
        const double T_next = T_prev + numExamples;
        const double eta = 1.0 / _parameters.regularization / T_prev;
        const double sigma = std::log(T_next) + 0.5 / T_next;

        // calulate the contribution of the old lastPredictor to the new avergedPredictor
        const double historyWeight = sigma - std::log(T_prev) - 0.5 / T_prev;
        math::Operations::Add(historyWeight, vLast, vAvg);
        bAvg += bLast * historyWeight;
        while (exampleIterator.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;
            const auto& dataVector = example.GetDataVector();

            // calculate the prediction
            double alpha = T_prev / (t - 1) * _lastPredictor.Predict(dataVector);

            // calculate the loss derivative
            double beta = weight * _lossFunction.GetDerivative(alpha, label);

            // Update vLast and vAvg
            double lastCoeff = -eta * beta;
            auto vLastTranspose = vLast.Transpose();
            dataVector.AddTo(vLastTranspose, lastCoeff);
            bLast += lastCoeff;

            double avgCoeff = lastCoeff * (sigma - std::log(t) - 0.5 / t);
            auto vAvgTranspose = vAvg.Transpose();
            dataVector.AddTo(vAvgTranspose, avgCoeff);
            bAvg += avgCoeff;

            exampleIterator.Next();
        }

        assert((double)_total_iterations == T_next);

        // calculate w and w_avg
        double scale = T_prev / T_next;
        math::Operations::Multiply(scale, vLast);
        bLast *= scale;
        math::Operations::Multiply(scale, vAvg);
        bAvg *= scale;
    }

    template <typename LossFunctionType>
    void SGDIncrementalTrainer<LossFunctionType>::UpdateDense(data::ExampleIterator<data::AutoSupervisedExample> exampleIterator)
    {
        // get references to the vector and biases
        auto& vLast = _lastPredictor.GetWeights();
        auto& vAvg = _averagedPredictor->GetWeights();

        double& bLast = _lastPredictor.GetBias();
        double& bAvg = _averagedPredictor->GetBias();

        while (exampleIterator.IsValid())
        {
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;
            const auto& dataVector = example.GetDataVector();

            // predict
            double alpha = _lastPredictor.Predict(dataVector);

            // calculate the loss derivative
            double beta = weight * _lossFunction.GetDerivative(alpha, label);

            // update last
            double scaleCoefficient = 1.0 - 1.0 / t;
            math::Operations::Multiply(scaleCoefficient, vLast);
            bLast *= scaleCoefficient;

            double updateCoefficient = -beta / t / _parameters.regularization;
            auto vLastTranspose = vLast.Transpose();
            dataVector.AddTo(vLastTranspose, updateCoefficient);
            bLast += updateCoefficient;

            // update average
            double averageingCoefficient = (t - 1) / t;
            math::Operations::Multiply(averageingCoefficient, vAvg);
            bAvg *= averageingCoefficient;
            // vLast.AddTo(vAvg, 1 / t); // dense operation
            bAvg += bLast / t;

            exampleIterator.Next();
        }
    }
}
}
