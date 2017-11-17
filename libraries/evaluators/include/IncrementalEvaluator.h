////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IncrementalEvaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Evaluator.h"

// stl
#include <memory>
#include <vector>

namespace ell
{
namespace evaluators
{
    /// <summary> Interface to an incremental evaluator (used to evaluate ensembles). </summary>
    ///
    /// <typeparam name="BasePredictorType"> The predictor type. </typeparam>
    template <typename BasePredictorType>
    class IIncrementalEvaluator
    {
    public:
        virtual ~IIncrementalEvaluator() = default;

        /// <summary>
        /// Runs the given predictor on the evaluation set, invokes each of the aggregators on the output,
        /// and logs the result.
        /// </summary>
        ///
        /// <param name="basePredictor"> The base predictor. </param>
        /// <param name="basePredictorWeight"> The weight of the base predictor in the ensemble. </param>
        /// <param name="evaluationRescale"> A rescaling coefficient applied to the current predictions of the entire ensemble, but not recorded in the evaluator. </param>
        virtual void IncrementalEvaluate(const BasePredictorType& basePredictor, double basePredictorWeight = 1.0, double evaluationRescale = 1.0) = 0;

        /// <summary> Gets the goodness of the most recent evaluation, according to the first aggregator. </summary>
        ///
        /// <returns> The goodness of the most recent evaluation. </returns>
        virtual double GetGoodness() const = 0;

        /// <summary> Prints the logged evaluations to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        virtual void Print(std::ostream& os) const = 0;
    };

    template <typename BasePredictorType, typename... AggregatorTypes>
    class IncrementalEvaluator : public Evaluator<BasePredictorType, AggregatorTypes...>, public IIncrementalEvaluator<BasePredictorType>
    {
    public:
        using BaseClassType = Evaluator<BasePredictorType, AggregatorTypes...>;

        /// <summary> Constructs an instance of IncrementalEvaluator with a given data set and given aggregators. </summary>
        ///
        /// <param name="anyDataset"> An dataset. </param>
        /// <param name="evaluatorParameters"> The evaluation parameters. </param>
        /// <param name="aggregators"> The aggregators. </param>
        IncrementalEvaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators);

        /// <summary>
        /// Runs the given predictor on the evaluation set, increments cached outputs, invokes each of
        /// the aggregators, and logs the result.
        /// </summary>
        ///
        /// <param name="basePredictor"> The base predictor. </param>
        /// <param name="basePredictorWeight"> The weight of the base predictor in the ensemble. </param>
        /// <param name="evaluationRescale"> A rescaling coefficient applied to the current predictions of the entire ensemble, but not recorded in the evaluator. </param>
        void IncrementalEvaluate(const BasePredictorType& basePredictor, double basePredictorWeight = 1.0, double evaluationRescale = 1.0) override;

        /// <summary> Gets the goodness of the most recent evaluation, according to the first aggregator. </summary>
        ///
        /// <returns> The goodness of the most recent evaluation. </returns>
        double GetGoodness() const override;

        /// <summary> Prints the logged evaluations to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        void Print(std::ostream& os) const override;

    private:
        std::vector<double> _predictions;
    };

    /// <summary> Makes an incremental evaluator (used to evaluate ensembles). </summary>
    ///
    /// <typeparam name="BasePredictorType"> The predictor type. </typeparam>
    /// <typeparam name="AggregatorTypes"> The Aggregator types. </typeparam>
    /// <param name="exampleIterator"> An example iterator that represents the evaluation data. </param>
    /// <param name="aggregators"> The aggregators. </param>
    ///
    /// <returns> A unique_ptr to an IEvaluator. </returns>
    template <typename BasePredictorType, typename... AggregatorTypes>
    std::shared_ptr<IIncrementalEvaluator<BasePredictorType>> MakeIncrementalEvaluator(data::ExampleIterator<data::AutoSupervisedExample> exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators);
}
}

#include "../tcc/IncrementalEvaluator.tcc"
