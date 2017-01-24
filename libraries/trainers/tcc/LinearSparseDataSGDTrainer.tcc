////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearSparseDataSGDTrainer.tcc (trainers)
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
    LinearSparseDataSGDTrainer<LossFunctionType>::LinearSparseDataSGDTrainer(const LossFunctionType& lossFunction, const LinearSparseDataSGDTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters)
    {
    }

    // this code follows the notation in https://arxiv.org/abs/1612.09147
    template <typename LossFunctionType>
    void LinearSparseDataSGDTrainer<LossFunctionType>::Update(const data::AnyDataset& anyDataset)
    {
        const double lambda = _parameters.regularization;

        // get example iterator
        auto exampleIterator = anyDataset.GetExampleIterator<data::AutoSupervisedExample>();

        // first iteration handled separately
        if (_t == 0 && exampleIterator.IsValid())
        {
            ++_t;

            const auto& example = exampleIterator.Get();

            const auto& x = example.GetDataVector();
            double y = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;

            double g = weight * _lossFunction.GetDerivative(0, y);
            
            auto xSize = x.PrefixLength();
            if (xSize > _v.Size())
            {
                _v.Resize(xSize);
                _u.Resize(xSize);
            }

            _v.Transpose() += g * x;
            _a += g;
            _c = _a;
            _h = 1;

            exampleIterator.Next();
        }

        while (exampleIterator.IsValid())
        {
            ++_t;

            // get the Next example
            const auto& example = exampleIterator.Get();

            const auto& x = example.GetDataVector();
            double y = example.GetMetadata().label;
            double weight = example.GetMetadata().weight;

            double d = x * _v;
            double p = -(d + _a) / (lambda * (_t - 1));
            double g = weight * _lossFunction.GetDerivative(p, y);
            _v.Transpose() += g * x;
            _a += g;
            _u.Transpose() += _h * g * x;
            _c += _a / _t;
            _h += 1 / _t;

            exampleIterator.Next();
        }

        // calculate the predictors
        _lastPredictor.Resize(_v.Size());
        _lastPredictor.GetWeights().Set((-1 / (lambda * _t)) * _v);
        _lastPredictor.GetBias() = -_a / (lambda * _t);

        _averagedPredictor.Resize(_v.Size());
        _averagedPredictor.GetWeights().Set(-_h / (lambda * _t) * _v);
        _averagedPredictor.GetWeights() += 1 / (lambda * _t) * _u;
        _averagedPredictor.GetBias() = -_c / (lambda * _t);
    }

    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeLinearSparseDataSGDTrainer(const LossFunctionType& lossFunction, const LinearSparseDataSGDTrainerParameters& parameters)
    {
        return std::make_unique<LinearSparseDataSGDTrainer<LossFunctionType>>(lossFunction, parameters);
    }
}
}
