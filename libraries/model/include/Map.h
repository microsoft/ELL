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
#include "OutputPort.h"
#include "PortElements.h"

// dataset
#include "DenseDataVector.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"
#include "TupleWrapper.h"
#include "FunctionUtils.h"
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

    template <typename T>
    using NamedInput = std::tuple<std::string, InputNode<T>*>;

    template <typename T>
    NamedInput<T> MakeNamedInput(std::string name, InputNode<T>* node)
    {
        return NamedInput<T>(name, node);
    }

    template <typename T>
    using NamedOutput = std::tuple<std::string, PortElements<T>>;

    template <typename T>
    NamedOutput<T> MakeNamedOutput(std::string name, PortElements<T> outputs)
    {
        return NamedOutput<T>(name, outputs);
    }

    template <typename T>
    NamedOutput<T> MakeNamedOutput(std::string name, const OutputPort<T>& outputs)
    {
        return NamedOutput<T>(name, outputs);
    }

    // Helper functions
    template <typename WrappedType>
    auto UnwrapType(std::tuple<std::string, model::InputNode<WrappedType>*>)
    {
        return WrappedType{};
    }

    template <typename WrappedType>
    auto UnwrapType(std::tuple<std::string, model::PortElements<WrappedType>>)
    {
        return WrappedType{};
    }

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
        /// <param name="outputs"> A vector of the outputs this map generates </param>
        Map(const Model& model,
            const utilities::WrappedTuple<InputTypesTuple, NamedInput>& inputs,
            const utilities::WrappedTuple<OutputTypesTuple, NamedOutput>& outputs);

        /// <summary> Set inputs </summary>
        ///
        /// <typeparam name="InputTypes"> The datatypes of the input nodes </typeparam>
        /// <param name="inputs"> The inputs to be routed to the input nodes </param>
        template <typename... InputTypes>
        void SetInputValues(std::vector<InputTypes>... inputs)
		{
			SetInputTuple(std::tuple<std::vector<InputTypes>...>(inputs...));
		}

        /// <summary> Type alias for the tuple of vectors returned by `Compute` </summary>
        using ComputedOutputType = utilities::WrappedTuple<OutputTypesTuple, StdVector>; // typename TupleOfVectorsFromPortElements<OutputTypesTuple>::type;

        /// <summary> Computes the output of the map from its current input values </summary>
        ///
        /// <returns> A tuple of vectors of output values </returns>
        ComputedOutputType ComputeOutput() const;

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
        utilities::WrappedTuple<InputTypesTuple, PointerToInputNode> _inputs;
        utilities::WrappedTuple<OutputTypesTuple, PortElements> _outputs;

        // Adding to _inputs/_outputs and name->value maps
        template <size_t... Sequence>
        void AddInputs(std::index_sequence<Sequence...>,
                       const utilities::WrappedTuple<InputTypesTuple, NamedInput>& inputs);

        template <size_t... Sequence>
        void AddOutputs(std::index_sequence<Sequence...>,
                        const utilities::WrappedTuple<OutputTypesTuple, NamedOutput>& outputs);

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
        void SetInputTuple(const std::tuple<std::vector<InputNodeTypes>...>& inputTuple)
		{
			SetInputElementsHelper(std::index_sequence_for<InputNodeTypes...>(), inputTuple);
		}

        template <typename InputNodeType, typename InputType>
        void SetNodeInput(InputNode<InputNodeType>* inputNode, const InputType& inputValues);

        template <typename InputNodesTupleType, size_t... Sequence>
        void SetInputElementsHelper(std::index_sequence<Sequence...>, const InputNodesTupleType& inputValues);

        // Compute
        template <typename PortElementsType, typename OutputType>
        void ComputeElements(PortElementsType& elements, OutputType& output) const;

        template <size_t... Sequence>
        void ComputeElementsHelper(std::index_sequence<Sequence...>, ComputedOutputType& outputValues) const;

        // Serialization helpers
        void PopulateInputs();
        void PopulateOutputs();

        template <size_t... Sequence>
        void PopulateInputsHelper(std::index_sequence<Sequence...>);

        template <size_t... Sequence>
        void PopulateOutputsHelper(std::index_sequence<Sequence...>);
    };

    template <typename NamedInputTypesTuple, typename NamedOutputTypesTuple>
    auto MakeMap(const Model& model,
                 const NamedInputTypesTuple& inputs,
                 const NamedOutputTypesTuple& outputs);
}
}

#include "../tcc/Map.tcc"
