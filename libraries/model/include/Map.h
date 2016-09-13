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
    //
    // General "wrap tuple types" mechanism
    //
    template <template <typename> class WrapperType, typename... Types>
    struct TupleOfWrappedElements
    {
        using type = std::tuple<WrapperType<Types>...>;
    };
    
    template <typename TupleType, template <typename> class WrapperType, size_t... Sequence>
    static auto MakeTupleOfThingsFromTypeTupleHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        // fails if Wrapper<T> has no copy constructor
        return typename TupleOfWrappedElements<WrapperType, typename std::tuple_element<Sequence, TupleType>::type...>::type();
    }
    
    template <typename TupleType, template <typename> class WrapperType>
    static auto MakeTupleOfThingsFromTypeTuple(const TupleType& tuple)
    {
        // fails if Wrapper<T> has no copy constructor
        return MakeTupleOfThingsFromTypeTupleHelper<TupleType, WrapperType>(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }
    
    template <typename TupleType, template <typename> class WrapperType>
    struct TupleTypeWrapper
    {
        using type = decltype(MakeTupleOfThingsFromTypeTuple<TupleType, WrapperType>(TupleType{}));
    };
    
    template <typename TupleType, template <typename> class WrapperType>
    using WrappedTuple = typename TupleTypeWrapper<TupleType, WrapperType>::type;

    //
    // Example usage:
    //
    // This is just to strip the extra template parameters from std::vector
    template <typename ValueType>
    using MyVector = std::vector<ValueType>;
    
    // WrappedTuple<std::tuple<int, float, double>, MyVector> x;  // x is a tuple<vector<int>, vector<float>, vector<double>>
    
    // TODO: replace special-case VectorWrapper with the above
    
    
    // Variadic helper class to transform input types into a std::tuple<std::vector<T1>, std::vector<T2>, ...>
    template <typename... Types>
    struct ToVectorElements
    {
        using type = std::tuple<std::vector<Types>...>;
    };

    // Here, TupleType is a tuple<PortElements<T1>, PortElements<T2>, ...>
    template <typename TupleType, size_t... Sequence>
    static auto MakeTupleOfVectorsFromPortElementsHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        using ElementType = typename std::tuple_element<Sequence..., TupleType>::type; // Here, ElementType is PortElements<T>
        return typename ToVectorElements<typename ElementType::value_type>::type{};

//        return typename ToVectorElements<typename std::tuple_element<Sequence..., TupleType>::type::type>::type{}; // compiles on Windows, not Mac
    }

    template <typename TupleType>
    static auto MakeTupleOfVectorsFromPortElements(const TupleType& tuple)
    {
        return MakeTupleOfVectorsFromPortElementsHelper(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    // Converts from a tuple of PortElements<T>s tuple of std::vector<T>s
    template <typename TupleType>
    struct TupleOfVectorsFromPortElements
    {
        using type = decltype(MakeTupleOfVectorsFromPortElements(TupleType{}));
    };

    // InputTypesTuple is really a tuple<InputNode<T1>*, InputNode<T2>*, ...>
    // OutputTypesTuple is really a tuple<PortElements<T1>, PortElements<T2>, ...>
    
    // TODO: make them instead just be tuple<T1, T2, ...>, and use some fancy thing to derive
    //       the tuple<InputNode<T>*, ...>, etc.
        
    /// <summary> Class that wraps a model and its designated outputs </summary>
    template <typename InputTypesTuple, typename OutputTypesTuple>
    class Map
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap </param>
        /// <param name="inputs"> A vector of the inputs this map uses </param>
        /// <param name="inputNames"> A vector of names for the inputs this map uses </param>
        /// <param name="outputs"> A vector of the outputs this map generates </param>
        /// <param name="outputNames"> A vector of names for the outputs this map generates </param>
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

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
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
        ComputeOutputType ComputeOutput() const;

        /// <summary> Computes of one of the map's outputs from its current input values </summary>
        ///
        /// <returns> A vector of output values </returns>
        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const std::string& outputName);

    private:
        Model _model;
        
        // TODO: Use the following, once we get InputTypesTuple to be a tuple oftypes, not a tuple of InputNode specializations
        template <typename T>
        using PointerToInputNode = InputNode<T>*;
        WrappedTuple<std::tuple<int, float, double>, PointerToInputNode> _inputs2;
        WrappedTuple<std::tuple<int, float, double>, PortElements> _outputs2;
        
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