////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Evaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"
#include "SupervisedExample.h"

// stl
#include <memory>
#include <vector>
#include <tuple>

namespace evaluators
{
    /// <summary> Interface to an evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    template <typename PredictorType>
    class IEvaluator
    {
    public:

        virtual ~IEvaluator() = default;

        /// <summary> Runs the given predictor on the evaluation set, invokes each of the aggregators on the output, and logs the result. </summary>
        ///
        /// <param name="predictor"> The predictor. </param>
        virtual void Evaluate(const PredictorType& predictor) = 0;

        /// <summary> Gets the goodness of the most recent evaluation, according to the first aggregator. </summary>
        ///
        /// <returns> The goodness of the most recent evaluation. </returns>
        virtual double GetGoodness() const = 0;

        /// <summary> Prints the logged evaluations to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        virtual void Print(std::ostream& os) const = 0;
    };

    /// <summary> Evaluator parameters. </summary>
    struct EvaluatorParameters
    {
        uint64_t evaluationFrequency;
        bool addZeroEvaluation;
    };

    /// <summary> Implements an evaluator that holds a dataset and a set of evaluation aggregators. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    /// <typeparam name="AggregatorTypes"> The aggregator types. </typeparam>
    template<typename PredictorType, typename... AggregatorTypes>
    class Evaluator : public IEvaluator<PredictorType>
    {
    public:

        /// <summary> Constructs an instance of Evaluator with a given dataset and given aggregators. </summary>
        ///
        /// <param name="exampleIterator"> An example iterator that represents the evaluation set. </param>
        /// <param name="evaluatorParameters"> The evaluation parameters. </param>
        /// <param name="aggregators"> The aggregators. </param>
        Evaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators);

        /// <summary> Runs the given predictor on the evaluation set, invokes each of the aggregators on the output, and logs the result. </summary>
        ///
        /// <param name="predictor"> The predictor. </param>
        virtual void Evaluate(const PredictorType& predictor) override;

        /// <summary> Gets the goodness of the most recent evaluation, according to the first aggregator. </summary>
        ///
        /// <returns> The goodness of the most recent evaluation. </returns>
        virtual double GetGoodness() const override;

        /// <summary> Prints the logged evaluations to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        virtual void Print(std::ostream& os) const override;

    protected:
        void EvaluateZero();

        template<std::size_t ...Is>
        void DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Is...>);

        template<std::size_t ...Is>
        void Aggregate(std::index_sequence<Is...>);

        template<std::size_t ...Is>
        void PrintDispatch(std::ostream& os, std::index_sequence<Is...>) const;

        // member variables
        dataset::GenericRowDataset _rowDataset;
        EvaluatorParameters _evaluatorParameters;
        uint64_t _evaluateCounter = 0;
        typename std::tuple<AggregatorTypes...> _aggregatorTuple;
        std::vector<std::tuple<typename AggregatorTypes::Value...>> _valueTuples;
    };

    /// <summary> Makes an evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    /// <typeparam name="AggregatorTypes"> The Aggregator types. </typeparam>
    /// <param name="exampleIterator"> An example iterator that represents the evaluation data. </param>
    /// <param name="aggregators"> The aggregators. </param>
    ///
    /// <returns> A unique_ptr to an IEvaluator. </returns>
    template<typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators);
}

#include "../tcc/Evaluator.tcc"