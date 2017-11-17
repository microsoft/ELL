////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Evaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <functional>
#include <memory>
#include <tuple>
#include <vector>

namespace ell
{
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
        size_t evaluationFrequency;
        bool addZeroEvaluation;
    };

    /// <summary> Implements an evaluator that holds a data set and a set of evaluation aggregators. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    /// <typeparam name="AggregatorTypes"> The aggregator types. </typeparam>
    template <typename PredictorType, typename... AggregatorTypes>
    class Evaluator : public IEvaluator<PredictorType>
    {
    public:
        /// <summary>
        /// Constructs an instance of Evaluator with a given data set and given aggregators.
        /// </summary>
        ///
        /// <param name="anyDataset"> A dataset. </param>
        /// <param name="evaluatorParameters"> The evaluation parameters. </param>
        /// <param name="aggregators"> The aggregators. </param>
        Evaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators);

        /// <summary> Runs the given predictor on the evaluation set, invokes each of the aggregators on the output, and logs the result. </summary>
        ///
        /// <param name="predictor"> The predictor. </param>
        void Evaluate(const PredictorType& predictor) override;

        /// <summary> Gets the goodness of the most recent evaluation, according to the first aggregator. </summary>
        ///
        /// <returns> The goodness of the most recent evaluation. </returns>
        double GetGoodness() const override;

        /// <summary> Returns a vector of names that describe the evaluation values represented in this Evaluator. </summary>
        ///
        /// <returns> A vector of names. </returns>
        std::vector<std::vector<std::string>> GetValueNames() const;

        /// <summary> Returns a constant reference to a datastructure V, where V[i][j][k] is the k'th value
        /// produced by the j'th aggregator on the i'th evaluation. </summary>
        ///
        /// <returns> A reference to the evaluation values. </returns>
        const std::vector<std::vector<std::vector<double>>>& GetValues() const { return _values; }

        /// <summary> Prints the logged evaluations to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        void Print(std::ostream& os) const override;

    protected:
        void EvaluateZero();

        template <size_t Index>
        using AggregatorType = typename std::tuple_element<Index, std::tuple<AggregatorTypes...>>::type;

        struct ElementUpdaterParameters
        {
            double prediction;
            double label;
            double weight;
        };

        template <typename AggregatorT>
        class ElementUpdater
        {
        public:
            ElementUpdater(AggregatorT& aggregator, const ElementUpdaterParameters& params);
            void operator()();

        private:
            ElementUpdaterParameters _params;
            AggregatorT& _aggregator;
        };

        template <typename AggregatorT>
        class ElementResetter
        {
        public:
            ElementResetter(AggregatorT& aggregator);

            void operator()();

        private:
            AggregatorT& _aggregator;
        };

        template <std::size_t Index>
        auto GetElementUpdateFunction(const ElementUpdaterParameters& params) -> ElementUpdater<AggregatorType<Index>>;

        template <std::size_t Index>
        auto GetElementResetFunction() -> ElementResetter<AggregatorType<Index>>;

        template <std::size_t... Sequence>
        void DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Sequence...>);

        template <std::size_t... Sequence>
        void Aggregate(std::index_sequence<Sequence...>);

        template <std::size_t... Sequence>
        std::vector<std::vector<std::string>> DispatchGetValueNames(std::index_sequence<Sequence...>) const;

        // the type of example used by this evaluator
        using ExampleType = data::Example<typename PredictorType::DataVectorType, data::WeightLabel>;

        // member variables
        data::Dataset<ExampleType> _dataset;
        EvaluatorParameters _evaluatorParameters;
        size_t _evaluateCounter = 0;
        typename std::tuple<AggregatorTypes...> _aggregatorTuple;
        std::vector<std::vector<std::vector<double>>> _values;
    };

    /// <summary> Makes an evaluator. </summary>
    ///
    /// <typeparam name="PredictorType"> The predictor type. </typeparam>
    /// <typeparam name="AggregatorTypes"> The Aggregator types. </typeparam>
    /// <param name="dataset"> A dataset. </param>
    /// <param name="aggregators"> The aggregators. </param>
    ///
    /// <returns> A shared_ptr to an IEvaluator. </returns>
    template <typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IEvaluator<PredictorType>> MakeEvaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators);
}
}

#include "../tcc/Evaluator.tcc"
