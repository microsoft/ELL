////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StochasticGradientDescentTrainer.tcc (trainers)
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
    StochasticGradientDescentTrainer<LossFunctionType>::StochasticGradientDescentTrainer(size_t dim, const LossFunctionType& lossFunction, const StochasticGradientDescentTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters), _total_iterations(0), _averagedPredictor(std::make_shared<PredictorType>(0))
    { // TODO dim is ignored
    }

    template <typename LossFunctionType>
    void StochasticGradientDescentTrainer<LossFunctionType>::Update(const data::AnyDataset& anyDataset)
    {
        UpdateSparse(anyDataset.GetExampleIterator<data::AutoSupervisedExample>(), anyDataset.NumExamples());
    }

    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeStochasticGradientDescentTrainer(size_t dim, const LossFunctionType& lossFunction, const StochasticGradientDescentTrainerParameters& parameters)
    {
        return std::make_unique<StochasticGradientDescentTrainer<LossFunctionType>>(dim, lossFunction, parameters);
    }

    template <typename LossFunctionType>
    void StochasticGradientDescentTrainer<LossFunctionType>::UpdateSparse(data::ExampleIterator<data::AutoSupervisedExample> exampleIterator, size_t numExamples)
    {
        // this code follows the notation in https://arxiv.org/abs/1612.09147

        // get references to the vector and biases
        auto& lastV = _lastPredictor.GetWeights();
        auto& averagedV = _averagedPredictor->GetWeights();

        double& lastB = _lastPredictor.GetBias();
        double& averagedB = _averagedPredictor->GetBias();

        // first iteration handled separately
        if (_total_iterations == 0)
        {
            const auto& example = exampleIterator.Get();

            const auto& dataVector = example.GetDataVector();
            double label = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;

            double g = _lossFunction.GetDerivative(0, label);
            dataVector.AddTo(lastV.Transpose(), g);
            dataVector.AddTo(averagedV.Transpose(), g);

            ++_total_iterations;
        }



        // define some constants
        const double T_prev = double(_total_iterations);
        const double T_next = T_prev + numExamples;
        const double eta = 1.0 / _parameters.regularization / T_prev;
        const double sigma = std::log(T_next) + 0.5 / T_next;

        // calulate the contribution of the old lastPredictor to the new avergedPredictor
        const double historyWeight = sigma - std::log(T_prev) - 0.5 / T_prev;
        math::Operations::Add(historyWeight, lastV, averagedV);
        averagedB += lastB * historyWeight;
        
        
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

            // Update lastV and averagedV
            double lastCoeff = -eta * beta;
            auto lastVTranspose = lastV.Transpose();
            dataVector.AddTo(lastVTranspose, lastCoeff);
            lastB += lastCoeff;

            double avgCoeff = lastCoeff * (sigma - std::log(t) - 0.5 / t);
            auto averagedVTranspose = averagedV.Transpose();
            dataVector.AddTo(averagedVTranspose, avgCoeff);
            averagedB += avgCoeff;

            exampleIterator.Next();
        }

        assert((double)_total_iterations == T_next);

        // calculate w and w_avg
        double scale = T_prev / T_next;
        math::Operations::Multiply(scale, lastV);
        lastB *= scale;
        math::Operations::Multiply(scale, averagedV);
        averagedB *= scale;
    }

    template <typename LossFunctionType>
    void StochasticGradientDescentTrainer<LossFunctionType>::UpdateDense(data::ExampleIterator<data::AutoSupervisedExample> exampleIterator)
    {
        // get references to the vector and biases
        auto& lastV = _lastPredictor.GetWeights();
        auto& averagedV = _averagedPredictor->GetWeights();

        double& lastB = _lastPredictor.GetBias();
        double& averagedB = _averagedPredictor->GetBias();

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
            math::Operations::Multiply(scaleCoefficient, lastV);
            lastB *= scaleCoefficient;

            double updateCoefficient = -beta / t / _parameters.regularization;
            auto lastVTranspose = lastV.Transpose();
            dataVector.AddTo(lastVTranspose, updateCoefficient);
            lastB += updateCoefficient;

            // update average
            double averageingCoefficient = (t - 1) / t;
            math::Operations::Multiply(averageingCoefficient, averagedV);
            averagedB *= averageingCoefficient;
            // lastV.AddTo(averagedV, 1 / t); // dense operation
            averagedB += lastB / t;

            exampleIterator.Next();
        }
    }
}
}
