////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDTrainer.tcc (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cassert>
#include <cmath>

// data
#include "DataVector.h"
#include "Dataset.h"
#include "DataVectorOperations.h"

// math
#include "VectorOperations.h"

namespace ell
{
namespace trainers
{
    // the code in this file follows the notation and pseudocode in https://arxiv.org/abs/1612.09147

    //
    // SGDTrainer
    //

    template <typename LossFunctionType>
    SGDTrainer<LossFunctionType>::SGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters)
        : SGDTrainerBase(parameters.randomSeedString), _lossFunction(lossFunction), _parameters(parameters)
    {
    }

    template<typename LossFunctionType>
    void SGDTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector& x, double y, double weight)
    {
        DoNextStep(x, y, weight);
    }

    template<typename LossFunctionType>
    void SGDTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector& x, double y, double weight)
    {
        ResizeTo(x);
        ++_t;

        // Predict
        double p = _lastPredictor.Predict(x);

        // calculate the loss derivative
        double g = weight * _lossFunction.GetDerivative(p, y);

        // get abbreviated names
        auto& lastW = _lastPredictor.GetWeights();
        double& lastB = _lastPredictor.GetBias();

        // update the (last) predictor
        double scaleCoefficient = 1.0 - 1.0 / _t;
        lastW *= scaleCoefficient;
        lastB *= scaleCoefficient;

        const double lambda = _parameters.regularization;
        double updateCoefficient = -g / (lambda * _t);
        lastW.Transpose() += updateCoefficient * x;
        lastB += updateCoefficient;

        // get abbreviated names
        auto& averagedW = _averagedPredictor.GetWeights();
        double& averagedB = _averagedPredictor.GetBias();

        // update the average predictor
        averagedW *= scaleCoefficient;
        averagedB *= scaleCoefficient;

        averagedW += 1.0 / _t * lastW;
        averagedB += lastB / _t;
    }

    template <typename LossFunctionType>
    void SGDTrainer<LossFunctionType>::ResizeTo(const data::AutoDataVector& x)
    {
        auto xSize = x.PrefixLength();
        if (xSize > _lastPredictor.Size())
        {
            _lastPredictor.Resize(xSize);
            _averagedPredictor.Resize(xSize);
        }
    }

    //
    // SparseDataSGDTrainer
    // 

    template<typename LossFunctionType>
    SparseDataSGDTrainer<LossFunctionType>::SparseDataSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters)
        : SGDTrainerBase(parameters.randomSeedString), _lossFunction(lossFunction), _parameters(parameters)
    {
    }

    template<typename LossFunctionType>
    void SparseDataSGDTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector& x, double y, double weight)
    {
        ResizeTo(x);
        _t = 1.0;
        double g = weight * _lossFunction.GetDerivative(0, y);
        _v.Transpose() += g * x;
        _a += g;
        _c = _a;
        _h = 1.0;
    }

    template<typename LossFunctionType>
    void SparseDataSGDTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector& x, double y, double weight)
    {
        ResizeTo(x);
        ++_t;

        // apply the predictor
        const double lambda = _parameters.regularization;
        double d = x * _v;
        double p = -(d + _a) / (lambda * (_t-1.0));

        // get the derivative
        double g = weight * _lossFunction.GetDerivative(p, y);
        
        // update
        _v.Transpose() += g * x;
        _a += g;
        _u.Transpose() += _h * g * x;
        _c += _a / _t;
        _h += 1.0 / _t;
    }

    template<typename LossFunctionType>
    auto SparseDataSGDTrainer<LossFunctionType>::GetLastPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _lastPredictor.Resize(_v.Size());
        auto& w = _lastPredictor.GetWeights();

        // define last predictor based on _v, _a, _t
        w.Reset();
        w += (-1 / (lambda * _t)) * _v;
        _lastPredictor.GetBias() = -_a / (lambda * _t);
        return _lastPredictor;
    }

    template<typename LossFunctionType>
    auto SparseDataSGDTrainer<LossFunctionType>::GetAveragedPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _averagedPredictor.Resize(_v.Size());
        auto& w = _averagedPredictor.GetWeights();

        // define averaged predictor based on _v, _h, _u, _t
        w.Reset();
        w += -_h / (lambda * _t) * _v;
        w += 1 / (lambda * _t) * _u;
        
        _averagedPredictor.GetBias() = -_c / (lambda * _t);
        return _averagedPredictor;
    }

    template <typename LossFunctionType>
    inline void SparseDataSGDTrainer<LossFunctionType>::ResizeTo(const data::AutoDataVector& x)
    {
        auto xSize = x.PrefixLength();
        if (xSize > _v.Size())
        {
            _v.Resize(xSize);
            _u.Resize(xSize);
        }
    }

    //
    // SparseDataCenteredSGDTrainer
    // 

    template<typename LossFunctionType>
    SparseDataCenteredSGDTrainer<LossFunctionType>::SparseDataCenteredSGDTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDTrainerParameters& parameters)
        : SGDTrainerBase(parameters.randomSeedString), _lossFunction(lossFunction), _parameters(parameters), _center(std::move(center))
    {
        _theta = 1 + _center.Norm2Squared();
    }

    template<typename LossFunctionType>
    void SparseDataCenteredSGDTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector& x, double y, double weight) 
    {
        ResizeTo(x);
        _t = 1.0;

        // first, perform the standard SparseDataSGD step
        double g = weight * _lossFunction.GetDerivative(0, y);
        _v.Transpose() += g * x;
        _a += g;
        _c = _a;
        _h = 1.0;

        // next, perform the special steps needed for centering
        double q = x * _center.Transpose();
        _z = g * q;
        _r = _a * _theta - _z;
        _s = _r;
    }

    template<typename LossFunctionType>
    void SparseDataCenteredSGDTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector& x, double y, double weight) 
    { 
        ResizeTo(x);
        ++_t;

        // apply the predictor
        const double lambda = _parameters.regularization;
        double d = x * _v;
        double q = x * _center.Transpose();
        double p = -(d + _r - _a * q) / (lambda * (_t-1.0));

        // get the derivative
        double g = weight * _lossFunction.GetDerivative(p, y);

        // apply the SparseDataSGD update
        _v.Transpose() += g * x;
        _a += g;
        _u.Transpose() += _h * g * x;
        _c += _a / _t;
        _h += 1.0 / _t;

        // next, perform the special steps needed for centering
        _z += g * q;
        _r = _a * _theta - _z;
        _s += _r / _t;
    }

    template<typename LossFunctionType>
    auto SparseDataCenteredSGDTrainer<LossFunctionType>::GetLastPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _lastPredictor.Resize(_v.Size());
        auto& w = _lastPredictor.GetWeights();
        w += (-1 / (lambda * _t)) * _v;
        _lastPredictor.GetBias() = -_a / (lambda * _t);
        return _lastPredictor;
    }

    template<typename LossFunctionType>
    auto SparseDataCenteredSGDTrainer<LossFunctionType>::GetAveragedPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        const double coeff = 1.0 / (lambda * _t);
        _averagedPredictor.Resize(_v.Size());
        auto& w = _averagedPredictor.GetWeights();

        // define last predictor based on _v, _u, _c
        w.Reset();
        w += -_h * coeff * _v;
        w += coeff * _u;
        w += _c * coeff * _center.Transpose();

        _averagedPredictor.GetBias() = -_s * coeff;
        return _averagedPredictor;
    }

    template <typename LossFunctionType>
    inline void SparseDataCenteredSGDTrainer<LossFunctionType>::ResizeTo(const data::AutoDataVector& x)
    {
        auto xSize = x.PrefixLength();
        if (xSize > _v.Size())
        {
            _v.Resize(xSize);
            _u.Resize(xSize);
        }
    }

    //
    // Helper functions
    //

    template <typename LossFunctionType>
    std::unique_ptr<ITrainer<predictors::LinearPredictor<double>>> MakeSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters)
    {
        return std::make_unique<SGDTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    template <typename LossFunctionType>
    std::unique_ptr<ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters)
    {
        return std::make_unique<SparseDataSGDTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    template <typename LossFunctionType>
    std::unique_ptr<ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataCenteredSGDTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDTrainerParameters& parameters)
    {
        return std::make_unique<SparseDataCenteredSGDTrainer<LossFunctionType>>(lossFunction, std::move(center), parameters);
    }
}
}
