////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DynamicMap.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "PortElements.h"

// dataset
#include "DenseDataVector.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"
#include "TupleWrapper.h"
#include "TypeName.h"


// stl
#include <array>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility> // for integer_sequence
#include <vector>

namespace emll
{
namespace model
{
    // This is just to strip the extra template parameters from std::vector so we can use the WrappedTuple<> helper
    template <typename ValueType>
    using StdVector = std::vector<ValueType>;

    // This typedef is so we can pass "PointerToInputNode" into the WrappedTuple<> helper
    template <typename T>
    using PointerToInputNode = InputNode<T>*;

    /// <summary> Class that wraps a model and its designated outputs </summary>
    template <typename InputTypesTuple, typename OutputTypesTuple>
    class Map : public DynamicMap
    {
    public:
        Map() = default;

        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap </param>
        /// <param name="inputs"> A vector of the inputs this map uses </param>
        /// <param name="inputNames"> A vector of names for the inputs this map uses </param>
        /// <param name="outputs"> A vector of the outputs this map generates </param>
        /// <param name="outputNames"> A vector of names for the outputs this map generates </param>
        Map(const Model& model,
            const utilities::WrappedTuple<InputTypesTuple, PointerToInputNode>& inputs,
            const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
            const utilities::WrappedTuple<OutputTypesTuple, PortElements>& outputs,
            const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames);

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
        // template <typename ValueType>
        // void SetInput(const std::string& inputName, const std::vector<ValueType>& inputValues);

        /// <summary> Type alias for the tuple of vectors returned by `Compute` </summary>
        using ComputeOutputType = utilities::WrappedTuple<OutputTypesTuple, StdVector>; // typename TupleOfVectorsFromPortElements<OutputTypesTuple>::type;

        /// <summary> Computes the output of the map from its current input values </summary>
        ///
        /// <returns> A tuple of vectors of output values </returns>
        ComputeOutputType ComputeOutput() const;

        using DynamicMap::ComputeOutput;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputTypesTuple, OutputTypesTuple>("Map"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Reads from a Unarchiver. </summary>
        ///
        /// <param name="archiver"> The archiver. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        /// <summary> Refines the model wrapped by this map </summary>
        virtual ModelTransformer DoRefine(const TransformContext& context) override;

    private:
        // maybe this should be a tuple<pair<string, WrappedTuple<T1>>, pair<string, WrappedTuple<T2>, ...>
        // We'd need a helper class thing to construct the type
        utilities::WrappedTuple<InputTypesTuple, PointerToInputNode> _inputs;
        utilities::WrappedTuple<OutputTypesTuple, PortElements> _outputs;

        // Adding to name->value maps
        template <size_t... Sequence>
        void AddInputsToNameMap(std::index_sequence<Sequence...>,
                                utilities::WrappedTuple<InputTypesTuple, PointerToInputNode>& inputs,
                                const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames);

        template <size_t... Sequence>
        void AddOutputsToNameMap(std::index_sequence<Sequence...>,
                                 utilities::WrappedTuple<OutputTypesTuple, PortElements>& outputs,
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

        // Serialization helpers
        void PopulateInputs();
        void PopulateOutputs();

        template <size_t... Sequence>
        void PopulateInputsHelper(std::index_sequence<Sequence...>);

        template <size_t... Sequence>
        void PopulateOutputsHelper(std::index_sequence<Sequence...>);
    };

    template <typename WrappedInputTypesTuple, typename WrappedOutputTypesTuple>
    auto MakeMap(const Model& model,
                 const WrappedInputTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<WrappedInputTypesTuple>::value>& inputNames,
                 const WrappedOutputTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<WrappedOutputTypesTuple>::value>& outputNames);
}
}

#include "../tcc/Map.tcc"
