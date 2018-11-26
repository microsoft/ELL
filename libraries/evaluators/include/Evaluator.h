////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Evaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BinaryErrorAggregator.h"

#include <data/include/Dataset.h>
#include <data/include/Example.h>

#include <utilities/include/FunctionUtils.h>

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
} // namespace evaluators
} // namespace ell

#pragma region implementation

namespace ell
{
namespace evaluators
{
    template <typename PredictorType, typename... AggregatorTypes>
    Evaluator<PredictorType, AggregatorTypes...>::Evaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators) :
        _dataset(anyDataset),
        _evaluatorParameters(evaluatorParameters),
        _aggregatorTuple(std::make_tuple(aggregators...))
    {
        static_assert(sizeof...(AggregatorTypes) > 0, "Evaluator must contains at least one aggregator");

        if (_evaluatorParameters.addZeroEvaluation)
        {
            EvaluateZero();
        }
    }

    template <typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Evaluate(const PredictorType& predictor)
    {
        ++_evaluateCounter;
        if (_evaluateCounter % _evaluatorParameters.evaluationFrequency != 0)
        {
            return;
        }

        auto iterator = _dataset.GetExampleReferenceIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetMetadata().weight;
            double label = example.GetMetadata().label;
            double prediction = predictor.Predict(example.GetDataVector());

            DispatchUpdate(prediction, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template <typename PredictorType, typename... AggregatorTypes>
    double Evaluator<PredictorType, AggregatorTypes...>::GetGoodness() const
    {
        if (_values.size() == 0)
        {
            return 0.0;
        }
        return _values.back()[0][0];
    }

    template <typename T>
    void PrintVector(std::ostream& os, const std::vector<T>& v)
    {
        if (v.size() == 0) return;

        os << v[0];
        for (size_t j = 1; j < v.size(); ++j)
        {
            os << '\t' << v[j];
        }
    }

    template <typename T>
    std::vector<T> FlattenJaggedVector(const std::vector<std::vector<T>>& v)
    {
        std::vector<T> concat;
        auto iter = v.cbegin();
        auto end = v.end();
        while (iter != end)
        {
            concat.insert(concat.end(), iter->cbegin(), iter->cend());
            ++iter;
        }
        return concat;
    }

    template <typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Print(std::ostream& os) const
    {
        auto originalPrecision = os.precision(6);
        auto originalFlags = os.setf(std::ios::fixed);

        PrintVector(os, FlattenJaggedVector(GetValueNames()));

        for (const auto& values : _values)
        {
            os << std::endl;
            PrintVector(os, FlattenJaggedVector(values));
        }

        os.setf(originalFlags);
        os.precision(originalPrecision);
    }

    template <typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::EvaluateZero()
    {
        auto iterator = _dataset.GetExampleIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetMetadata().weight;
            double label = example.GetMetadata().label;

            DispatchUpdate(0.0, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <typename AggregatorT>
    Evaluator<PredictorType, AggregatorTypes...>::ElementUpdater<AggregatorT>::ElementUpdater(AggregatorT& aggregator, const ElementUpdaterParameters& params) :
        _params(params),
        _aggregator(aggregator)
    {
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <typename AggregatorT>
    void Evaluator<PredictorType, AggregatorTypes...>::ElementUpdater<AggregatorT>::operator()()
    {
        _aggregator.Update(_params.prediction, _params.label, _params.weight);
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <typename AggregatorT>
    Evaluator<PredictorType, AggregatorTypes...>::ElementResetter<AggregatorT>::ElementResetter(AggregatorT& aggregator) :
        _aggregator(aggregator)
    {
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <typename AggregatorT>
    void Evaluator<PredictorType, AggregatorTypes...>::ElementResetter<AggregatorT>::operator()()
    {
        _aggregator.Reset();
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t Index>
    auto Evaluator<PredictorType, AggregatorTypes...>::GetElementUpdateFunction(const ElementUpdaterParameters& params) -> ElementUpdater<AggregatorType<Index>>
    {
        return { std::get<Index>(_aggregatorTuple), params };
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t Index>
    auto Evaluator<PredictorType, AggregatorTypes...>::GetElementResetFunction() -> ElementResetter<AggregatorType<Index>>
    {
        return { std::get<Index>(_aggregatorTuple) };
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t... Sequence>
    void Evaluator<PredictorType, AggregatorTypes...>::DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Sequence...>)
    {
        // Call (X.Update(), 0) for each X in _aggregatorTuple
        ElementUpdaterParameters params{ prediction, label, weight };
        utilities::InOrderFunctionEvaluator(GetElementUpdateFunction<Sequence>(params)...);
        // [this, prediction, label, weight]() { std::get<Sequence>(_aggregatorTuple).Update(prediction, label, weight); }...); // GCC bug prevents compilation
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t... Sequence>
    void Evaluator<PredictorType, AggregatorTypes...>::Aggregate(std::index_sequence<Sequence...>)
    {
        // Call X.GetResult() for each X in _aggregatorTuple
        _values.push_back({ std::get<Sequence>(_aggregatorTuple).GetResult()... });

        // Call X.Reset() for each X in _aggregatorTuple
        utilities::InOrderFunctionEvaluator(GetElementResetFunction<Sequence>()...);
        // utilities::InOrderFunctionEvaluator([this]() { std::get<Sequence>(_aggregatorTuple).Reset(); }...); // GCC bug prevents compilation
    }

    template <typename PredictorType, typename... AggregatorTypes>
    std::vector<std::vector<std::string>> Evaluator<PredictorType, AggregatorTypes...>::GetValueNames() const
    {
        return DispatchGetValueNames(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t... Sequence>
    std::vector<std::vector<std::string>> Evaluator<PredictorType, AggregatorTypes...>::DispatchGetValueNames(std::index_sequence<Sequence...>) const
    {
        return { std::get<Sequence>(_aggregatorTuple).GetValueNames()... };
    }

    template <typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IEvaluator<PredictorType>> MakeEvaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
    {
        return std::make_unique<Evaluator<PredictorType, AggregatorTypes...>>(anyDataset, evaluatorParameters, aggregators...);
    }
} // namespace evaluators
} // namespace ell

#pragma endregion implementation
