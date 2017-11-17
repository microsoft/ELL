////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDTrainer.h (trainers)
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
}
}

#include "../tcc/SGDTrainer.tcc"
