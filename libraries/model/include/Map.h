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

// stl
#include <string>
#include <vector>
#include <array>
#include <utility> // for integer_sequence

#include <iostream>

namespace emll
{
namespace model
{
    template <typename T>
    struct VectorOf
    {
        using type = std::vector<T>;
    };

    template <typename... Types>
    struct TupleOfVectors
    {
        using type = std::tuple<typename VectorOf<Types>::type...>;
    };

    template <typename TupleType, size_t... Sequence>
    auto MakeTupleOfVectorsFromTypeTupleHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        return typename TupleOfVectors<typename std::tuple_element<Sequence, TupleType>::type...>::type();
    }

    template <typename TupleType>
    auto MakeTupleOfVectorsFromTypeTuple(const TupleType& tuple)
    {
        return MakeTupleOfVectorsFromTypeTupleHelper(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    // TupleType is a tuple<PortElements<T1>, PortElements<T2>, ...>
    template <typename TupleType, size_t... Sequence>
    auto MakeTupleOfVectorsFromPortElementsTupleHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        return typename TupleOfVectors<typename std::tuple_element<Sequence, TupleType>::type::type...>::type();
    }

    template <typename TupleType>
    auto MakeTupleOfVectorsFromPortElementsTuple(const TupleType& tuple)
    {
        return MakeTupleOfVectorsFromPortElementsTupleHelper(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    template <typename TupleType>
    struct TupleVectorMaker
    {
        using type = decltype(MakeTupleOfVectorsFromTypeTuple(TupleType{}));
    };

    template <typename TupleType>
    struct TupleVectorMakerFromPortElements
    {
        using type = decltype(MakeTupleOfVectorsFromPortElementsTuple(TupleType{}));
    };

    /// <summary> Class that wraps a model and its designated outputs </summary>
    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    class Map // : utilities::IArchivable
    {
    public:
        Map(const Model& model,
            const InputNodeTypesTuple& inputs,
            const std::array<std::string, std::tuple_size<InputNodeTypesTuple>::value>& inputNames,
            const OutputPortTypesTuple& outputs,
            const std::array<std::string, std::tuple_size<OutputPortTypesTuple>::value>& outputNames);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputNodeTypesTuple, OutputPortTypesTuple>("Map"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        const Model& GetModel() const { return _model; }

        /// <summary> Refines the model wrapped by this map </summary>
        void Refine(const TransformContext& context);

        // <summary> Set inputs </summary>
        template <typename... InputNodeTypes>
        void SetInputs(const std::tuple<std::vector<InputNodeTypes>...>& inputValues);

        /// <summary> Computes the output of the map from its current input values </summary>
        typename TupleVectorMakerFromPortElements<OutputPortTypesTuple>::type Compute() const;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        // virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        // virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        Model _model;

        std::array<std::string, std::tuple_size<InputNodeTypesTuple>::value> _inputNames;
        InputNodeTypesTuple _inputs; // this is a tuple of InputNode<T>*

        std::array<std::string, std::tuple_size<OutputPortTypesTuple>::value> _outputNames;
        OutputPortTypesTuple _outputs; // This is a tuple of PortElements<T>

        // Remap
        template <typename OutputElementsType>
        void RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer);

        template <size_t... Sequence>
        void RemapOutputElementsHelper(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer);

        // SetInput
        template <typename InputNodeType, typename InputType>
        void SetNodeInput(InputNode<InputNodeType>* inputNode, const InputType& inputValues);

        template <typename InputNodesTupleType, size_t... Sequence>
        void SetInputElementsHelper(std::index_sequence<Sequence...>, const InputNodesTupleType& inputValues);

        // Compute
        template <typename PortElementsType, typename OutputType>
        void ComputeElements(PortElementsType& elements, OutputType& output) const;

        template <size_t... Sequence>
        void ComputeElementsHelper(std::index_sequence<Sequence...>, typename TupleVectorMakerFromPortElements<OutputPortTypesTuple>::type& outputValues) const;
    };

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    auto MakeMap(const Model& model,
                 const InputNodeTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<InputNodeTypesTuple>::value>& inputNames,
                 const OutputPortTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<OutputPortTypesTuple>::value>& outputNames);
}
}

#include "../tcc/Map.tcc"