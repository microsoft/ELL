////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "TypeName.h"
#include "IArchivable.h"
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <array>
#include <utility> // for integer_sequence
#include <unordered_map>

namespace emll
{
namespace model
{
    // Variadic helper class to transform input types into a std::tuple<std::vector<T1>, std::vector<T2>, ...>
    template <typename... Types>
    struct ToVectorElements
    {
        using type = std::tuple<std::vector<Types>...>;
    };

    // Here, TupleType is a tuple<PortElements<T1>, PortElements<T2>, ...>
    template <typename TupleType, size_t... Sequence>
    auto MakeTupleOfVectorsFromPortElementsHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        return typename ToVectorElements<typename std::tuple_element<Sequence, TupleType>::type::type...>::type();
    }

    template <typename TupleType>
    auto MakeTupleOfVectorsFromPortElements(const TupleType& tuple)
    {
        return MakeTupleOfVectorsFromPortElementsHelper(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    // Converts from a tuple of PortElements<T>s tuple of std::vector<T>s
    template <typename TupleType>
    struct TupleOfVectorsFromPortElements
    {
        using type = decltype(MakeTupleOfVectorsFromPortElements(TupleType{}));
    };

    /// <summary> Class that wraps a model and its designated outputs </summary>
    template <typename InputTypesTuple, typename OutputTypesTuple>
    class Map
    {
    public:
        Map(const Model& model,
            const InputTypesTuple& inputs,
            const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
            const OutputTypesTuple& outputs,
            const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputTypesTuple, OutputTypesTuple>("Map"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        const Model& GetModel() const { return _model; }

        /// <summary> Refines the model wrapped by this map </summary>
        void Refine(const TransformContext& context);

        /// <summary> Set inputs </summary>
        ///
        /// <typeparam name="InputTypes"> The datatypes of the input nodes </typeparam>
        /// <param name="inputs"> The inputs to be routed to the input nodes </param>
        template <typename... InputTypes>
        void SetInputs(std::vector<InputTypes>... inputs);

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="inputName"> The name assigned to the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename ValueType>
        void SetInput(const std::string& inputName, const std::vector<ValueType>& inputValues);

        /// <summary> Type alias for the tuple of vectors returned by `Compute` </summary>
        using ComputeOutputType = typename TupleOfVectorsFromPortElements<OutputTypesTuple>::type;

        /// <summary> Computes the output of the map from its current input values </summary>
        ///
        /// <returns> A tuple of vectors of output values </returns>
        ComputeOutputType Compute() const;

        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const std::string& outputName);

    private:
        Model _model;

        InputTypesTuple _inputs; // InputTypesTuple is a std::tuple<InputNode<T1>*, InputNode<T2>*, ...>
        std::array<std::string, std::tuple_size<InputTypesTuple>::value> _inputNames;
        std::unordered_map<std::string, Node*> _inputNodeMap;

        OutputTypesTuple _outputs; // OutputTypesTuple is a std::tuple<PortElements<T1>, PortElements<T2>, ...>
        std::array<std::string, std::tuple_size<OutputTypesTuple>::value> _outputNames;
        std::unordered_map<std::string, const PortElementsBase&> _outputElementsMap;

        // Adding to name->value maps
        template <size_t... Sequence>
        void AddInputsToNameMap(std::index_sequence<Sequence...>,
                                InputTypesTuple& inputs,
                                const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames);

        template <size_t... Sequence>
        void AddOutputsToNameMap(std::index_sequence<Sequence...>,
                                 OutputTypesTuple& outputs,
                                 const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames);

        // Remap
        template <typename InputNodeType>
        void RemapInputNode(InputNodeType& input, ModelTransformer& modelTransformer);

        template <size_t... Sequence>
        void RemapInputNodes(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer);

        template <typename OutputElementsType>
        void RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer);

        template <size_t... Sequence>
        void RemapOutputElements(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer);

        // SetInput
        template <typename... InputNodeTypes>
        void SetInputTuple(const std::tuple<std::vector<InputNodeTypes>...>& inputTuple);

        template <typename InputNodeType, typename InputType>
        void SetNodeInput(InputNode<InputNodeType>* inputNode, const InputType& inputValues);

        template <typename InputNodesTupleType, size_t... Sequence>
        void SetInputElementsHelper(std::index_sequence<Sequence...>, const InputNodesTupleType& inputValues);

        // Compute
        template <typename PortElementsType, typename OutputType>
        void ComputeElements(PortElementsType& elements, OutputType& output) const;

        template <size_t... Sequence>
        void ComputeElementsHelper(std::index_sequence<Sequence...>, ComputeOutputType& outputValues) const;
    };

    template <typename InputTypesTuple, typename OutputTypesTuple>
    auto MakeMap(const Model& model,
                 const InputTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
                 const OutputTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames);
}
}

#include "../tcc/Map.tcc"