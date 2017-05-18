////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDLinearTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

// predictors
#include "LinearPredictor.h"

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <cstddef>
#include <memory>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the stochastic gradient descent trainer. </summary>
    struct SGDLinearTrainerParameters
    {
        double regularization;
    };

    /// <summary>
    /// Implements the averaged stochastic gradient descent algorithm on an L2 regularized empirical
    /// loss. This class must be have a derived class that implements DoFirstStep(), DoNextStep(), and CalculatePredictors().
    /// </summary>
    /// <typeparam name="DerivedType"> The derived class (curiously recurring template pattern). </typeparam>
    class SGDLinearTrainerBase : public ITrainer<predictors::LinearPredictor>
    {
    public:
        using PredictorType = predictors::LinearPredictor;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        virtual void Update(const data::AnyDataset& anyDataset) override;

        /// <summary> Returns The averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        virtual const predictors::LinearPredictor& GetPredictor() const override { return GetAveragedPredictor(); }

    protected:
        // Instances of the base class cannot be created directly
        SGDLinearTrainerBase() = default;
        virtual void DoFirstStep(const data::AutoDataVector& x, double y, double weight) = 0;
        virtual void DoNextStep(const data::AutoDataVector& x, double y, double weight) = 0;
        virtual const PredictorType& GetAveragedPredictor() const = 0;

        bool _firstIteration = true;
    };

    //
    // SGDLinearTrainer - Stochastic Gradient Descent
    //

    /// <summary> Implements the steps of a simple sgd linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename LossFunctionType>
    class SGDLinearTrainer : public SGDLinearTrainerBase
    {
    public:
        using SGDLinearTrainerBase::PredictorType;

        /// <summary> Constructs an SGD linear trainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> The training parameters. </param>
        SGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters);

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const { return _lastPredictor; }

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        virtual const PredictorType& GetAveragedPredictor() const override { return _averagedPredictor; }

    protected:
        virtual void DoFirstStep(const data::AutoDataVector& x, double y, double weight) override;
        virtual void DoNextStep(const data::AutoDataVector& x, double y, double weight) override;

    private:
        LossFunctionType _lossFunction;
        SGDLinearTrainerParameters _parameters;

        double _t = 0; // step counter;
        PredictorType _lastPredictor;
        PredictorType _averagedPredictor;

        void ResizeTo(const data::AutoDataVector& x);
    };

    //
    // SDSGDLinearTrainer - Sparse Data Stochastic Gradient Descent
    //

    /// <summary> Implements the steps of Sparse Data Stochastic Gradient Descent. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename LossFunctionType>
    class SDSGDLinearTrainer : public SGDLinearTrainerBase
    {
    public:
        using SGDLinearTrainerBase::PredictorType;

        /// <summary> Constructs an instance of SDSGDLinearTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="parameters"> The training parameters. </param>
        SDSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters);

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const;

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        virtual const PredictorType& GetAveragedPredictor() const override;

    protected:
        virtual void DoFirstStep(const data::AutoDataVector& x, double y, double weight) override;
        virtual void DoNextStep(const data::AutoDataVector& x, double y, double weight) override;

    private:
        LossFunctionType _lossFunction;
        SGDLinearTrainerParameters _parameters;

        // these variables follow the notation in https://arxiv.org/abs/1612.09147
        math::ColumnVector<double> _v;  // gradient sum - weights
        math::ColumnVector<double> _u;  // harmonic-weighted gradient sum - weights
        double _t = 0;                  // step counter
        double _a = 0;                  // gradient sum - bias
        double _h = 0;                  // harmonic number
        double _c = 0;                  // 1/t-weighted sum of _a

        // these variables are mutable because we calculate them in a lazy manner (only when `GetPredictor() const` is called)
        mutable PredictorType _lastPredictor;
        mutable PredictorType _averagedPredictor;

        void ResizeTo(const data::AutoDataVector& x);
    };

    //
    // SDCSGDLinearTrainer - Sparse Data Centered Stochastic Gradient Descent
    //

    /// <summary> Implements the steps of Sparse Data Centered Stochastic Gradient Descent. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    template <typename LossFunctionType>
    class SDCSGDLinearTrainer : public SGDLinearTrainerBase
    {
    public:
        using SGDLinearTrainerBase::PredictorType;

        /// <summary> Constructs an instance of SDCSGDLinearTrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="center"> The center (mean) of the training set. </param>
        /// <param name="parameters"> Trainer parameters. </param>
        SDCSGDLinearTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDLinearTrainerParameters& parameters);

        /// <summary> Returns a const reference to the last predictor. </summary>
        ///
        /// <returns> A const reference to the last predictor. </returns>
        const PredictorType& GetLastPredictor() const;

        /// <summary> Returns a const reference to the averaged predictor. </summary>
        ///
        /// <returns> A const reference to the averaged predictor. </returns>
        virtual const PredictorType& GetAveragedPredictor() const override;

    protected:
        virtual void DoFirstStep(const data::AutoDataVector& x, double y, double weight) override;
        virtual void DoNextStep(const data::AutoDataVector& x, double y, double weight) override;

    private:
        LossFunctionType _lossFunction;
        SGDLinearTrainerParameters _parameters;

        // these variables follow the notation in https://arxiv.org/abs/1612.09147
        math::ColumnVector<double> _v;  // gradient sum - weights
        math::ColumnVector<double> _u;  // harmonic-weighted gradient sum - weights
        double _t = 0;                  // step counter
        double _a = 0;                  // gradient sum - bias
        double _h = 0;                  // harmonic number
        double _c = 0;                  // 1/t-weighted sum of _a

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
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters);

    /// <summary> Makes a SDSGD linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSDSGDLinearTrainer(const LossFunctionType& lossFunction, const SGDLinearTrainerParameters& parameters);

    /// <summary> Makes a SDCSGD linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="parameters"> The trainer parameters. </param>
    /// <param name="center"> The center (mean) of the training set. </param>
    /// <param name="lossFunction"> The loss function. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor>> MakeSDCSGDLinearTrainer(const LossFunctionType& lossFunction, math::RowVector<double> center, const SGDLinearTrainerParameters& parameters);
}
}

#include "../tcc/SGDLinearTrainer.tcc"
