////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearSGDTrainer.tcc (trainers)
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
    LinearSGDTrainer<LossFunctionType>::LinearSGDTrainer(size_t dim, const LossFunctionType& lossFunction, const LinearSGDTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters), _total_iterations(0), _averagedPredictor(std::make_shared<PredictorType>(0))
    { // TODO dim is ignored
    }

    template <typename LossFunctionType>
    void LinearSGDTrainer<LossFunctionType>::Update(const data::AnyDataset& anyDataset)
    {
        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

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

    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeLinearSGDTrainer(size_t dim, const LossFunctionType& lossFunction, const LinearSGDTrainerParameters& parameters)
    {
        return std::make_unique<LinearSGDTrainer<LossFunctionType>>(dim, lossFunction, parameters);
    }
}
}
