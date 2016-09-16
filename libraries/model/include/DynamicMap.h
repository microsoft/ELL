////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DynamicMap.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "InputNode.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// dataset
#include "DenseDataVector.h"

// utilities
#include "TypeName.h"
#include "TypeTraits.h"
#include "IArchivable.h"

// stl
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

namespace emll
{
namespace model
{
    /// <summary> Class that wraps a model and its designated outputs </summary>
    class DynamicMap : public utilities::IArchivable
    {
    public:
        /// <summary> Constructor </summary>
        DynamicMap() = default;

        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap </param>
        /// <param name="inputs"> A vector of name/value pairs for the inputs this map uses </param>
        /// <param name="outputs"> A vector of name/value pairs for the outputs this map generates </param>
        DynamicMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs);

        /// <summary> Gets the model wrapped by this map </summary>
        ///
        /// <returns> The `Model` </returns>
        const Model& GetModel() const { return _model; }

        /// <summary> Refines the model wrapped by this map </summary>
        void Refine(const TransformContext& context);

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="inputName"> The name assigned to the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename ValueType>
        void SetInput(const std::string& inputName, const std::vector<ValueType>& inputValues);

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="inputName"> The name assigned to the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename ValueType>
        void SetInput(const std::string& inputName, const dataset::DenseDataVector<ValueType>& inputValues);

        /// <summary> Computes of one of the map's outputs from its current input values </summary>
        ///
        /// <returns> A vector of output values </returns>
        template <typename ValueType, utilities::IsFundamental<ValueType> = 0>
        std::vector<ValueType> ComputeOutput(const std::string& outputName);

        template <typename VectorType, typename ValueType = typename VectorType::value_type>
        VectorType ComputeOutput(const std::string& outputName);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "DynamicMap"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Reads from a Unarchiver. </summary>
        ///
        /// <param name="archiver"> The archiver. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        Model _model;
        std::unordered_map<std::string, InputNodeBase*> _inputNodeMap;
        std::unordered_map<std::string, PortElementsBase> _outputElementsMap;
    };

    /// <summary> A serialization context used during model deserialization. Wraps an existing `SerializationContext`
    /// and adds access to the model being constructed. </summary>
    class DynamicMapSerializationContext : public ModelSerializationContext
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="previousContext"> The `SerializationContext` to wrap </param>
        /// <param name="model"> The model being constructed </param>
        DynamicMapSerializationContext(utilities::SerializationContext& previousContext);
    };
}
}

#include "../tcc/DynamicMap.tcc"