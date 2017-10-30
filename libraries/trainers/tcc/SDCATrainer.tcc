////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SDCATrainer.tcc (trainers)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVectorOperations.h"

// utilities
#include "RandomEngines.h"

namespace ell
{
namespace trainers
{
    template<typename LossFunctionType, typename RegularizerType>
    SDCATrainer<LossFunctionType, RegularizerType>::SDCATrainer(const LossFunctionType& lossFunction, const RegularizerType& regularizer, const SDCATrainerParameters& parameters)
    : _lossFunction(lossFunction), _regularizer(regularizer), _parameters(parameters)
    {
        _random = utilities::GetRandomEngine(parameters.randomSeedString);
    }

    template<typename LossFunctionType, typename RegularizerType>
    void SDCATrainer<LossFunctionType, RegularizerType>::SetDataset(const data::AnyDataset& anyDataset)
    {
        DEBUG_THROW(_v.Norm0() != 0, utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "can only call SetDataset before updates"));

        _dataset = data::Dataset<TrainerExampleType>(anyDataset);
        auto numExamples = _dataset.NumExamples();
        _inverseScaledRegularization = 1.0 / (numExamples * _parameters.regularization);

        _predictorInfo.primalObjective = 0;
        _predictorInfo.dualObjective = 0;

        // precompute the norm of each example
        for (size_t rowIndex = 0; rowIndex < numExamples; ++rowIndex)
        {
            auto& example = _dataset[rowIndex];
            example.GetMetadata().norm2Squared = example.GetDataVector().Norm2Squared();

            auto label = example.GetMetadata().weightLabel.label;
            _predictorInfo.primalObjective += _lossFunction(0, label) / numExamples;
        }
    }

    template<typename LossFunctionType, typename RegularizerType>
    void SDCATrainer<LossFunctionType, RegularizerType>::Update() 
    {
        if (_parameters.permute)
        {
            _dataset.RandomPermute(_random);
        }

        // Iterate
        for (size_t i = 0; i < _dataset.NumExamples(); ++i)
        {
            Step(_dataset[i]);
        }

        // Finish
        ComputeObjectives();
    }

    template<typename LossFunctionType, typename RegularizerType>
    SDCATrainer<LossFunctionType, RegularizerType>::TrainerMetadata::TrainerMetadata(const data::WeightLabel& original) : weightLabel(original)
    {}

    template<typename LossFunctionType, typename RegularizerType>
    void SDCATrainer<LossFunctionType, RegularizerType>::Step(TrainerExampleType& example)
    {
        const auto& dataVector = example.GetDataVector();
        ResizeTo(dataVector);

        auto weightLabel = example.GetMetadata().weightLabel;
        auto norm2Squared = example.GetMetadata().norm2Squared + 1; // add one because of bias term
        auto lipschitz = norm2Squared * _inverseScaledRegularization;
        auto dual = example.GetMetadata().dualVariable; 

        if (lipschitz > 0)
        {
            auto prediction = _predictor.Predict(dataVector);
            
            auto newDual = _lossFunction.ConjugateProx(1.0 / lipschitz, dual + prediction / lipschitz, weightLabel.label);
            auto dualDiff = newDual - dual;
            
            if (dualDiff != 0)
            {
                _v.Transpose() += (-dualDiff * _inverseScaledRegularization) * dataVector;
                _d += (-dualDiff * _inverseScaledRegularization);
                _regularizer.ConjugateGradient(_v, _d, _predictor.GetWeights(), _predictor.GetBias());
                example.GetMetadata().dualVariable = newDual;
            }
        }
    }

    template<typename LossFunctionType, typename RegularizerType>
    void SDCATrainer<LossFunctionType, RegularizerType>::ComputeObjectives()
    {
        double invSize = 1.0 / _dataset.NumExamples();

        _predictorInfo.primalObjective = 0;
        _predictorInfo.dualObjective = 0;

        for (size_t i = 0; i < _dataset.NumExamples(); ++i)
        {
            const auto& example = _dataset.GetExample(i);
            auto label = example.GetMetadata().weightLabel.label;
            auto prediction = _predictor.Predict(example.GetDataVector());
            auto dualVariable = example.GetMetadata().dualVariable;

            _predictorInfo.primalObjective += invSize * _lossFunction(prediction, label);
            _predictorInfo.dualObjective -= invSize * _lossFunction.Conjugate(dualVariable, label);
        }

        _predictorInfo.primalObjective += _parameters.regularization * _regularizer(_predictor.GetWeights(), _predictor.GetBias());
        _predictorInfo.dualObjective -= _parameters.regularization * _regularizer.Conjugate(_v, _d);
    }

    template<typename LossFunctionType, typename RegularizerType>
    void SDCATrainer<LossFunctionType, RegularizerType>::ResizeTo(const data::AutoDataVector& x)
    {
        auto xSize = x.PrefixLength();
        if (xSize > _predictor.Size())
        {
            _predictor.Resize(xSize);
            _v.Resize(xSize);
        }
    }

    template <typename LossFunctionType, typename RegularizerType>
    std::unique_ptr<trainers::ITrainer<predictors::LinearPredictor<double>>> MakeSDCATrainer(const LossFunctionType& lossFunction, const RegularizerType& regularizer, const SDCATrainerParameters& parameters)
    {
        return std::make_unique<SDCATrainer<LossFunctionType, RegularizerType>>(lossFunction, regularizer, parameters);
    }
}
}