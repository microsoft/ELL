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
#include "DataVectorOperators.h"

namespace ell
{
namespace trainers
{
    template <typename LossFunctionType>
    LinearSGDTrainer<LossFunctionType>::LinearSGDTrainer(const LossFunctionType& lossFunction, const LinearSGDTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters), _total_iterations(0), _averagedPredictor(std::make_shared<PredictorType>(0))
    {
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
            // get iteration index
            ++_total_iterations;
            double t = (double)_total_iterations;

            // get the next example
            const auto& example = exampleIterator.Get();
            double y = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;
            const auto& x = example.GetDataVector();

            // predict
            double p = _lastPredictor.Predict(x);

            // resize predictors as necessary
            auto xSize = x.PrefixLength();
            if (xSize > lastV.Size())
            {
                lastV.Resize(xSize);
                averagedV.Resize(xSize);
            }

            // calculate the loss derivative
            double g = weight * _lossFunction.GetDerivative(p, y);

            // update the (last) predictor
            double scaleCoefficient = 1.0 - 1.0 / t;
            lastV *= scaleCoefficient;
            lastB *= scaleCoefficient;

            double updateCoefficient = -g / t / _parameters.regularization;
            lastV.Transpose() += updateCoefficient * x;
            lastB += updateCoefficient;

            // update the average predictor
            averagedV *= scaleCoefficient;
            averagedB *= scaleCoefficient;

            averagedV += 1.0 / t * lastV;
            averagedB += lastB / t;

            exampleIterator.Next();
        }
    }

    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeLinearSGDTrainer(const LossFunctionType& lossFunction, const LinearSGDTrainerParameters& parameters)
    {
        return std::make_unique<LinearSGDTrainer<LossFunctionType>>(lossFunction, parameters);
    }
}
}
