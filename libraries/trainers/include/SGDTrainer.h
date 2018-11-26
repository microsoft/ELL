////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

#include <predictors/include/LinearPredictor.h>

#include <data/include/Dataset.h>
#include <data/include/Example.h>

#include <cstddef>
#include <memory>
#include <random>
#include <string>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the stochastic gradient descent trainer. </summary>
    struct SGDTrainerParameters
    {
        double regularization;
        std::string randomSeedString;
    };

    /// <summary>
    /// Implements the averaged stochastic gradient descent algorithm on an L2 regularized empirical
    /// loss. This class must be have a derived class that implements DoFirstStep(), DoNextStep(), and CalculatePredictors().
    /// </summary>
    class SGDTrainerBase : public ITrainer<predictors::LinearPredictor<double>>
    {
    public:
        using PredictorType = predictors::LinearPredictor<double>;

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        void Update() override;

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        const predictors::LinearPredictor<double>& GetPredictor() const override { return GetAveragedPredictor(); }

    protected:
        // Instances of the base class cannot be created directly
        SGDTrainerBase(std::string randomSeedString);
        virtual void DoFirstStep(const data::AutoDataVector& x, double y, double weight) = 0;
        virtual void DoNextStep(const data::AutoDataVector& x, double y, double weight) = 0;
        virtual const PredictorType& GetAveragedPredictor() const = 0;

        data::AutoSupervisedDataset _dataset;
        std::default_random_engine _random;
        bool _firstIteration = true;
    };

    //
    // SGDTrainer - Stochastic Gradient Descent
    //

    /// <summary> Implements the steps of a simple sgd linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename LossFunctionType>
    class SGDTrainer : public SGDTrainerBase
    {
    public:
        using SGDTrainerBase::PredictorType;

        /// <summary> Constructs an SGD linear trainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> The training parameters. </param>
        SGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters);

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const { return _lastPredictor; }

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        const PredictorType& GetAveragedPredictor() const override { return _averagedPredictor; }

    protected:
        void DoFirstStep(const data::AutoDataVector& x, double y, double weight) override;
        void DoNextStep(const data::AutoDataVector& x, double y, double weight) override;

    private:
        LossFunctionType _lossFunction;
        SGDTrainerParameters _parameters;

        double _t = 0; // step counter;
        PredictorType _lastPredictor;
        PredictorType _averagedPredictor;

        void ResizeTo(const data::AutoDataVector& x);
    };

    //
    // SparseDataSGDTrainer - Sparse Data Stochastic Gradient Descent
    //

    /// <summary> Implements the steps of Sparse Data Stochastic Gradient Descent. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename LossFunctionType>
    class SparseDataSGDTrainer : public SGDTrainerBase
    {
    public:
        using SGDTrainerBase::PredictorType;

        /// <summary> Constructs an instance of SparseDataSGDTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> The training parameters. </param>
        SparseDataSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters);

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const;

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        const PredictorType& GetAveragedPredictor() const override;

    protected:
        void DoFirstStep(const data::AutoDataVector& x, double y, double weight) override;
        void DoNextStep(const data::AutoDataVector& x, double y, double weight) override;

    private:
        LossFunctionType _lossFunction;
        SGDTrainerParameters _parameters;

        // these variables follow the notation in https://arxiv.org/abs/1612.09147
        math::ColumnVector<double> _v; // gradient sum - weights
        math::ColumnVector<double> _u; // harmonic-weighted gradient sum - weights
        double _t = 0; // step counter
        double _a = 0; // gradient sum - bias
        double _h = 0; // harmonic number
        double _c = 0; // 1/t-weighted sum of _a

        // these variables are mutable because we calculate them in a lazy manner (only when `GetPredictor() const` is called)
        mutable PredictorType _lastPredictor;
        mutable PredictorType _averagedPredictor;

        void ResizeTo(const data::AutoDataVector& x);
    };

    //
    // SparseDataCenteredSGDTrainer - Sparse Data Centered Stochastic Gradient Descent
    //

    /// <summary> Implements the steps of Sparse Data Centered Stochastic Gradient Descent. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename LossFunctionType>
    class SparseDataCenteredSGDTrainer : public SGDTrainerBase
    {
    public:
        using SGDTrainerBase::PredictorType;

        /// <summary> Constructs an instance of SparseDataCenteredSGDTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="center"> The center (mean) of the training set. </param>
        /// <param name="parameters"> Trainer parameters. </param>
        SparseDataCenteredSGDTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDTrainerParameters& parameters);

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const;

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        const PredictorType& GetAveragedPredictor() const override;

    protected:
        void DoFirstStep(const data::AutoDataVector& x, double y, double weight) override;
        void DoNextStep(const data::AutoDataVector& x, double y, double weight) override;

    private:
        LossFunctionType _lossFunction;
        SGDTrainerParameters _parameters;

        // these variables follow the notation in https://arxiv.org/abs/1612.09147
        math::ColumnVector<double> _v; // gradient sum - weights
        math::ColumnVector<double> _u; // harmonic-weighted gradient sum - weights
        double _t = 0; // step counter
        double _a = 0; // gradient sum - bias
        double _h = 0; // harmonic number
        double _c = 0; // 1/t-weighted sum of _a

        double _z = 0;
        double _r = 0;
        double _s = 0;

        math::RowVector<double> _center;
        double _theta;

        // these variables are mutable because we calculate them in a lazy manner (only when `GetPredictor() const` is called)
        mutable PredictorType _lastPredictor;
        mutable PredictorType _averagedPredictor;

        void ResizeTo(const data::AutoDataVector& x);
    };

    //
    // MakeTrainer helper functions
    //

    /// <summary> Makes a SGD linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters);

    /// <summary> Makes a SparseDataSGD linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters);

    /// <summary> Makes a SparseDataCenteredSGD linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="center"> The center (mean) of the training set. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSparseDataCenteredSGDTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDTrainerParameters& parameters);
} // namespace trainers
} // namespace ell

#pragma region implementation

#include <cmath>

#include <data/include/DataVector.h>
#include <data/include/DataVectorOperations.h>
#include <data/include/Dataset.h>

#include <math/include/VectorOperations.h>

namespace ell
{
namespace trainers
{
    // the code in this file follows the notation and pseudocode in https://arxiv.org/abs/1612.09147

    //
    // SGDTrainer
    //

    template <typename LossFunctionType>
    SGDTrainer<LossFunctionType>::SGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters) :
        SGDTrainerBase(parameters.randomSeedString),
        _lossFunction(lossFunction),
        _parameters(parameters)
    {
    }

    template <typename LossFunctionType>
    void SGDTrainer<LossFunctionType>::DoFirstStep(const data::AutoDataVector& x, double y, double weight)
    {
        DoNextStep(x, y, weight);
    }

    template <typename LossFunctionType>
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

    template <typename LossFunctionType>
    SparseDataSGDTrainer<LossFunctionType>::SparseDataSGDTrainer(const LossFunctionType& lossFunction, const SGDTrainerParameters& parameters) :
        SGDTrainerBase(parameters.randomSeedString),
        _lossFunction(lossFunction),
        _parameters(parameters)
    {
    }

    template <typename LossFunctionType>
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

    template <typename LossFunctionType>
    void SparseDataSGDTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector& x, double y, double weight)
    {
        ResizeTo(x);
        ++_t;

        // apply the predictor
        const double lambda = _parameters.regularization;
        double d = x * _v;
        double p = -(d + _a) / (lambda * (_t - 1.0));

        // get the derivative
        double g = weight * _lossFunction.GetDerivative(p, y);

        // update
        _v.Transpose() += g * x;
        _a += g;
        _u.Transpose() += _h * g * x;
        _c += _a / _t;
        _h += 1.0 / _t;
    }

    template <typename LossFunctionType>
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

    template <typename LossFunctionType>
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

    template <typename LossFunctionType>
    SparseDataCenteredSGDTrainer<LossFunctionType>::SparseDataCenteredSGDTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDTrainerParameters& parameters) :
        SGDTrainerBase(parameters.randomSeedString),
        _lossFunction(lossFunction),
        _parameters(parameters),
        _center(std::move(center))
    {
        _theta = 1 + _center.Norm2Squared();
    }

    template <typename LossFunctionType>
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

    template <typename LossFunctionType>
    void SparseDataCenteredSGDTrainer<LossFunctionType>::DoNextStep(const data::AutoDataVector& x, double y, double weight)
    {
        ResizeTo(x);
        ++_t;

        // apply the predictor
        const double lambda = _parameters.regularization;
        double d = x * _v;
        double q = x * _center.Transpose();
        double p = -(d + _r - _a * q) / (lambda * (_t - 1.0));

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

    template <typename LossFunctionType>
    auto SparseDataCenteredSGDTrainer<LossFunctionType>::GetLastPredictor() const -> const PredictorType&
    {
        const double lambda = _parameters.regularization;
        _lastPredictor.Resize(_v.Size());
        auto& w = _lastPredictor.GetWeights();
        w += (-1 / (lambda * _t)) * _v;
        _lastPredictor.GetBias() = -_a / (lambda * _t);
        return _lastPredictor;
    }

    template <typename LossFunctionType>
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
} // namespace trainers
} // namespace ell

#pragma endregion implementation
