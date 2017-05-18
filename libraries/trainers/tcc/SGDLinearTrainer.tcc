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
#include "DataVector.h"
#include "Dataset.h"
#include "DataVectorTransformations.h"

namespace ell
{
namespace trainers
{
    // the code in this file follows the notation and pseudocode in https://arxiv.org/abs/1612.09147

    //
    // SGDLinearTrainer
    //

    template <typename LossFunctionType>
    SGDLinearTrainer<LossFunctionType>::SGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters)
    {
    }

    template<typename LossFunctionType>
    void SGDLinearTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector & x, double y, double weight)
    {
        DoNextStep(x, y, weight);
    }

    template<typename LossFunctionType>
    void SGDLinearTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector & x, double y, double weight)
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
    inline void SGDLinearTrainer<LossFunctionType>::ResizeTo(const data::AutoDataVector& x)
    {
        auto xSize = x.PrefixLength();
        if (xSize > _lastPredictor.Size())
        {
            _lastPredictor.Resize(xSize);
            _averagedPredictor.Resize(xSize);
        }
    }

    //
    // SDSGDLinearTrainer
    // 

    template<typename LossFunctionType>
    SDSGDLinearTrainer<LossFunctionType>::SDSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters)
    {
    }

    template<typename LossFunctionType>
    void SDSGDLinearTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector & x, double y, double weight)
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
    void SDSGDLinearTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector & x, double y, double weight)
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
    auto SDSGDLinearTrainer<LossFunctionType>::GetLastPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _lastPredictor.Resize(_v.Size());
        _lastPredictor.GetWeights().Set((-1 / (lambda * _t)) * _v);
        _lastPredictor.GetBias() = -_a / (lambda * _t);
        return _lastPredictor;
    }

    template<typename LossFunctionType>
    auto SDSGDLinearTrainer<LossFunctionType>::GetAveragedPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _averagedPredictor.Resize(_v.Size());
        _averagedPredictor.GetWeights().Set(-_h / (lambda * _t) * _v);
        _averagedPredictor.GetWeights() += 1 / (lambda * _t) * _u;
        _averagedPredictor.GetBias() = -_c / (lambda * _t);
        return _averagedPredictor;
    }

    template <typename LossFunctionType>
    inline void SDSGDLinearTrainer<LossFunctionType>::ResizeTo(const data::AutoDataVector& x)
    {
        auto xSize = x.PrefixLength();
        if (xSize > _v.Size())
        {
            _v.Resize(xSize);
            _u.Resize(xSize);
        }
    }

    //
    // SDCSGDLinearTrainer
    // 

    template<typename LossFunctionType>
    SDCSGDLinearTrainer<LossFunctionType>::SDCSGDLinearTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDLinearTrainerParameters& parameters)
        : _lossFunction(lossFunction), _parameters(parameters), _center(std::move(center))
    {
        auto norm = _center.Norm2();
        _theta = 1 + norm * norm;
    }

    template<typename LossFunctionType>
    void SDCSGDLinearTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector & x, double y, double weight) 
    {
        ResizeTo(x);
        _t = 1.0;

        // first, perform the standard SDSGD step
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
    void SDCSGDLinearTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector & x, double y, double weight) 
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

        // apply the SDSGD update
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
    auto SDCSGDLinearTrainer<LossFunctionType>::GetLastPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _lastPredictor.Resize(_v.Size());
        _lastPredictor.GetWeights().Set((-1 / (lambda * _t)) * _v);
        _lastPredictor.GetBias() = -_a / (lambda * _t);
        return _lastPredictor;
    }

    template<typename LossFunctionType>
    auto SDCSGDLinearTrainer<LossFunctionType>::GetAveragedPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        const double coeff = 1.0 / (lambda * _t);
        _averagedPredictor.Resize(_v.Size());
        _averagedPredictor.GetWeights().Set(-_h * coeff * _v);
        _averagedPredictor.GetWeights() += coeff * _u;
        _averagedPredictor.GetWeights() += _c * coeff * _center.Transpose();

        _averagedPredictor.GetBias() = -_s * coeff;
        return _averagedPredictor;
    }

    template <typename LossFunctionType>
    inline void SDCSGDLinearTrainer<LossFunctionType>::ResizeTo(const data::AutoDataVector& x)
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
    std::unique_ptr<ITrainer<predictors::LinearPredictor>> MakeSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters)
    {
        return std::make_unique<SGDLinearTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    template <typename LossFunctionType>
    std::unique_ptr<ITrainer<predictors::LinearPredictor>> MakeSDSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters)
    {
        return std::make_unique<SDSGDLinearTrainer<LossFunctionType>>(lossFunction, parameters);
    }

    template <typename LossFunctionType>
    std::unique_ptr<ITrainer<predictors::LinearPredictor>> MakeSDCSGDLinearTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDLinearTrainerParameters& parameters)
    {
        return std::make_unique<SDCSGDLinearTrainer<LossFunctionType>>(lossFunction, std::move(center), parameters);
    }
}
}
