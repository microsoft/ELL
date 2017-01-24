////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDLinearTrainer.tcc (trainers)
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
    SGDLinearTrainer<LossFunctionType>::SGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters)
    {
    }

    template <typename LossFunctionType>
    void SGDLinearTrainer<LossFunctionType>::Update(const data::AnyDataset& anyDataset)
    {
        const double lambda = _parameters.regularization;

        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

        // get references to the vector and biases
        auto& lastW = _lastPredictor.GetWeights();
        auto& averagedW = _averagedPredictor.GetWeights();

        double& lastB = _lastPredictor.GetBias();
        double& averagedB = _averagedPredictor.GetBias();

        while (exampleIterator.IsValid())
        {
            // get iteration index
            ++_t;

            // get the next example
            const auto& example = exampleIterator.Get();
            const auto& x = example.GetDataVector();
            double y = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;

            // predict
            double p = _lastPredictor.Predict(x);

            // resize predictors as necessary
            auto xSize = x.PrefixLength();
            if (xSize > lastW.Size())
            {
                lastW.Resize(xSize);
                averagedW.Resize(xSize);
            }

            // calculate the loss derivative
            double g = weight * _lossFunction.GetDerivative(p, y);

            // update the (last) predictor
            double scaleCoefficient = 1.0 - 1.0 / _t;
            lastW *= scaleCoefficient;
            lastB *= scaleCoefficient;

            double updateCoefficient = -g / (lambda * _t);
            lastW.Transpose() += updateCoefficient * x;
            lastB += updateCoefficient;

            // update the average predictor
            averagedW *= scaleCoefficient;
            averagedB *= scaleCoefficient;

            averagedW += 1.0 / _t * lastW;
            averagedB += lastB / _t;

            exampleIterator.Next();
        }
    }

    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters)
    {
        return std::make_unique<SGDLinearTrainer<LossFunctionType>>(lossFunction, parameters);
    }
}
}
