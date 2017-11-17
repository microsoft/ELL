////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SDCATrainer.h (trainers)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ITrainer.h"

// predictors
#include "LinearPredictor.h"

// data
#include "Dataset.h"
#include "Example.h"

// math
#include "Vector.h"

// stl
#include <random>

namespace ell
{
namespace trainers
{
    /// <summary> Parameters for the stochastic dual coordinate ascent trainer. </summary>
    struct SDCATrainerParameters
    {
        double regularization;
        double desiredPrecision;
        size_t maxEpochs;
        bool permute;
        std::string randomSeedString;
    };

    /// <summary> Information about the result of an SDCA training session. </summary>
    struct SDCAPredictorInfo
    {
        double primalObjective = 0;
        double dualObjective = 0;
        size_t numEpochsPerformed = 0;
    };

    /// <summary> Implements the stochastic dual coordinate ascent linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Loss function type. </typeparam>
    /// <typeparam name="RegularizerType"> Regularizer type. </typeparam>
    template<typename LossFunctionType, typename RegularizerType>
    class SDCATrainer : public ITrainer<predictors::LinearPredictor<double>>
    {
    public:
        /// <summary> Constructs an instance of SDCATrainer. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        /// <param name="regularizer"> The regularizer. </param>
        /// <param name="parameters"> Trainer parameters. </param>
        SDCATrainer(const LossFunctionType& lossFunction, const RegularizerType& regularizer, const SDCATrainerParameters& parameters);

        /// <summary> Sets the trainer's dataset. </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        void SetDataset(const data::AnyDataset& anyDataset) override;

        /// <summary> Updates the state of the trainer by performing a learning epoch. </summary>
        void Update() override;

        /// <summary> Gets the trained predictor. </summary>
        ///
        /// <returns> A const reference to the predictor. </returns>
        const predictors::LinearPredictor<double>& GetPredictor() const override { return _predictor; }

        /// <summary> Gets information on the trained predictor. </summary>
        ///
        /// <returns> Information on the trained predictor. </returns>
        SDCAPredictorInfo GetPredictorInfo() const { return _predictorInfo; }

    private:
        struct TrainerMetadata
        {
            TrainerMetadata(const data::WeightLabel& weightLabel);

            // weight and label
            data::WeightLabel weightLabel;

            // precomputed squared 2 norm of the data vector 
            double norm2Squared = 0;

            // dual variable
            double dualVariable = 0;
        };

        using DataVectorType = typename predictors::LinearPredictor<double>::DataVectorType;
        using TrainerExampleType = data::Example<DataVectorType, TrainerMetadata>;

        void Step(TrainerExampleType& x);
        void ComputeObjectives();
        void ResizeTo(const data::AutoDataVector& x);

        LossFunctionType _lossFunction;
        RegularizerType _regularizer;
        SDCATrainerParameters _parameters;
        std::default_random_engine _random;
        double _inverseScaledRegularization;

        data::Dataset<TrainerExampleType> _dataset;

        predictors::LinearPredictor<double> _predictor;
        SDCAPredictorInfo _predictorInfo;

        math::ColumnVector<double> _v;
        double _d = 0;
        math::RowVector<double> _a;
    };

    //
    // MakeTrainer helper functions
    //

    /// <summary> Makes a SDCA linear trainer. </summary>
    ///
    /// <typeparam name="LossFunctionType"> Type of loss function to use. </typeparam>
    /// <param name="lossFunction"> The loss function. </param>
    /// <param name="parameters"> The trainer parameters. </param>
    ///
    /// <returns> A linear trainer </returns>
    template <typename LossFunctionType, typename RegularizerType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSDCATrainer(const LossFunctionType& lossFunction, const RegularizerType& regularizer, const SDCATrainerParameters& parameters);
}
}

#include "../tcc/SDCATrainer.tcc"
