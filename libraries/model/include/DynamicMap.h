////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DynamicMap.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputNode.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "PortElements.h"

// data
#include "DataVector.h"

// utilities
#include "TypeTraits.h"
#include "IArchivable.h"
#include "Exception.h"
#include "StlIndexValueIterator.h"

// stl
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

namespace emll
{
namespace model
{
    /// <summary> Class that wraps a model and its designated outputs </summary>
    class DynamicMap : public utilities::IArchivable
    {
    public:
        DynamicMap() = default;

        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap </param>
        DynamicMap(const Model& model);

        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap </param>
        /// <param name="inputs"> A vector of name/value pairs for the inputs this map uses </param>
        /// <param name="outputs"> A vector of name/value pairs for the outputs this map generates </param>
        DynamicMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs);

        virtual ~DynamicMap() = default;

        /// <summary> Gets the model wrapped by this map </summary>
        ///
        /// <returns> The `Model` </returns>
        const Model& GetModel() const { return _model; }

        /// <summary> Gets the model wrapped by this map </summary>
        ///
        /// <returns> The `Model` </returns>
        Model& GetModel() { return _model; }

        /// <summary> Refines the model wrapped by this map </summary>
        ///
        /// <param name="context"> The TransformContext to use during refinement </param>
        void Refine(const TransformContext& context);

        /// <summary> Returns the requested input node </summary>
        ///
        /// <param name="inputName"> The name of the input </param>
        /// <returns> The specified input node </returns>
        InputNodeBase* GetInputNode(int inputIndex) const;

        /// <summary> Returns the requested input node </summary>
        ///
        /// <param name="inputName"> The name of the input </param>
        /// <returns> The specified input node </returns>
        InputNodeBase* GetInputNode(const std::string& inputName) const;

        //
        // Routines for getting information about inputs / outputs of the map
        //

        /// <returns> The number of input nodes </returns>
        size_t NumInputs() const { return _inputNodes.size(); }

        /// <summary> Returns size of a given input </summary>
        ///
        /// <param name="inputIndex"> The index of the input </param>
        /// <returns> The size of the input </returns>
        size_t GetInputSize(int inputIndex) const;

        /// <summary> Returns size of a given input </summary>
        ///
        /// <param name="inputName"> The name of the input </param>
        /// <returns> The size of the input </returns>
        size_t GetInputSize(const std::string& inputName) const;

        /// <summary> Returns the input nodes </summary>
        ///
        /// <returns> The input nodes </returns>
        const std::vector<InputNodeBase*>& GetInputs() { return _inputNodes; }

        /// <summary> Returns an input node </summary>
        ///
        /// <param name="index"> The index of the input </param>
        /// <returns> The input node </returns>
        InputNodeBase* GetInput(size_t index) const;

        /// <summary> Returns an input node </summary>
        ///
        /// <param name="index"> The name of the input </param>
        /// <returns> The input node </returns>
        InputNodeBase* GetInput(const std::string& inputName) const;

        /// <summary> Get the number of outputs </summary>
        ///
        /// <returns> The number of outputs </returns>
        size_t NumOutputs() const { return _outputElements.size(); }

        /// <summary> Returns size of a given output </summary>
        ///
        /// <param name="outputName"> The name of the output </param>
        /// <returns> The dimensionality of the output </returns>
        size_t GetOutputSize(const std::string& outputName) const;

        /// <summary> Returns size of a given output </summary>
        ///
        /// <param name="outputIndex"> The index of the output </param>
        /// <returns> The dimensionality of the output </returns>
        size_t GetOutputSize(int outputIndex) const;

        /// <summary> Returns the outputs </summary>
        ///
        /// <returns> The outputs </returns>
        const std::vector<PortElementsBase>& GetOutputs() { return _outputElements; }

        /// <summary> Returns an outputs </summary>
        ///
        /// <param name="index"> The index of the output </param>
        /// <returns> The output </returns>
        PortElementsBase GetOutput(size_t index) const;

        /// <summary> Returns an outputs </summary>
        ///
        /// <param name="outputName"> The name of the output </param>
        /// <returns> The output </returns>
        PortElementsBase GetOutput(const std::string& outputName) const;

        //
        // Routines for computing output (processing data)
        //

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="inputName"> The name assigned to the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename ValueType>
        void SetInputValue(int index, const std::vector<ValueType>& inputValues) const;

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="inputName"> The name assigned to the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename ValueType>
        void SetInputValue(const std::string& inputName, const std::vector<ValueType>& inputValues) const;

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="index"> The index of the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename DataVectorType, data::IsDataVector<DataVectorType> Concept = true>
        void SetInputValue(int index, const DataVectorType& inputValues) const;

        /// <summary> Set a single InputNode's input </summary>
        ///
        /// <typeparam name="ValueType"> The datatype of the input node </typeparam>
        /// <param name="index"> The index of the input node </param>
        /// <param name="inputValues"> The values to set on the input node </param>
        template <typename DataVectorType, data::IsDataVector<DataVectorType> Concept = true>
        void SetInputValue(const std::string& inputName, const DataVectorType& inputValues) const;

        /// <summary> Computes of one of the map's outputs from its current input values </summary>
        ///
        /// <param name="index"> The index of the output </param>
        /// <returns> A vector of output values </returns>
        template <typename ValueType, utilities::IsFundamental<ValueType> = 0>
        std::vector<ValueType> ComputeOutput(int index) const;

        /// <summary> Computes of one of the map's outputs from its current input values </summary>
        ///
        /// <param name="index"> The index of the output </param>
        /// <returns> A vector of output values </returns>
        template <typename DataVectorType, data::IsDataVector<DataVectorType> Concept = true>
        DataVectorType ComputeOutput(int index) const;

        /// <summary> Computes of one of the map's outputs from its current input values </summary>
        ///
        /// <param name="outputName"> The name of the output </param>
        /// <returns> A vector of output values </returns>
        template <typename ValueType, utilities::IsFundamental<ValueType> = 0>
        std::vector<ValueType> ComputeOutput(const std::string& outputName) const;

        /// <summary> Computes of one of the map's outputs from its current input values </summary>
        ///
        /// <param name="outputName"> The name of the output </param>
        /// <returns> A vector of output values </returns>
        template <typename DataVectorType, data::IsDataVector<DataVectorType> Concept = true>
        DataVectorType ComputeOutput(const std::string& outputName) const;

        /// <summary> Returns a `PortElements` object representing the indicated map output </summary>
        ///
        /// <param name="outputIndex"> The zero-based index of the map output </param>
        /// <returns> The `PortElements` object representing the indicated outputs </returns>
        template <typename ValueType>
        PortElements<ValueType> GetOutputElements(size_t outputIndex);

        /// <summary> Returns a `PortElements` object representing the indicated map output </summary>
        ///
        /// <param name="outputName"> The name of the map output </param>
        /// <returns> The `PortElements` object representing the indicated outputs </returns>
        template <typename ValueType>
        PortElements<ValueType> GetOutputElements(std::string outputName);

        /// <summary> Returns a `PortElementsBase` object representing the indicated map output </summary>
        ///
        /// <param name="outputIndex"> The zero-based index of the map output </param>
        /// <returns> The `PortElementsBase` object representing the indicated outputs </returns>
        PortElementsBase GetOutputElementsBase(size_t outputIndex);

        /// <summary> Returns a `PortElementsBase` object representing the indicated map output </summary>
        ///
        /// <param name="outputName"> The name of the map output </param>
        /// <returns> The `PortElementsBase` object representing the indicated outputs </returns>
        PortElementsBase GetOutputElementsBase(const std::string& outputName);

        // TODO: add ComputeOutput(index, outvec) and ComputeOutput(name, outvec)

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "DynamicMap"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        // helper
        template <typename DataVectorType, typename ElementsType, data::IsDataVector<DataVectorType> Concept = true>
        void SetInputValue(InputNodeBase* node, const DataVectorType& inputValues) const;

        template <typename DataVectorType, data::IsDataVector<DataVectorType> Concept = true>
        void SetInputValue(InputNodeBase* node, const DataVectorType& inputValues) const;

        template <typename DataVectorType, typename ElementsType, data::IsDataVector<DataVectorType> Concept = true>
        DataVectorType ComputeOutput(const PortElementsBase& elements) const;

        template <typename ValueType, utilities::IsFundamental<ValueType> = 1>
        std::vector<ValueType> ComputeOutput(const PortElementsBase& elements) const;

        template <typename DataVectorType, data::IsDataVector<DataVectorType> Concept = true>
        DataVectorType ComputeOutput(const PortElementsBase& elements) const;

        /// <summary> Add an input to the map </summary>
        ///
        /// <param name="inputName"> The name to assign to the input node </param>
        /// <param name="inputNode"> The input node to add </param>
        void AddInput(const std::string& inputName, InputNodeBase* inputNode);
        void AddOutput(const std::string& outputName, PortElementsBase outputElements);
        void ResetOutput(size_t index, PortElementsBase outputElements);
        void Prune(); // prune away unused parts of internal model

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        virtual ModelTransformer DoPrune(); // overridable prune implementation
        virtual ModelTransformer DoRefine(const TransformContext& context); // overridable refine implementation

        virtual void SetNodeInput(InputNode<bool>* node, const std::vector<bool>& inputValues) const;
        virtual void SetNodeInput(InputNode<int>* node, const std::vector<int>& inputValues) const;
        virtual void SetNodeInput(InputNode<double>* node, const std::vector<double>& inputValues) const;

        virtual std::vector<bool> ComputeBoolOutput(const PortElementsBase& outputs);
        virtual std::vector<int> ComputeIntOutput(const PortElementsBase& outputs);
        virtual std::vector<double> ComputeDoubleOutput(const PortElementsBase& outputs);

    private:
        Model _model;

        std::vector<InputNodeBase*> _inputNodes;
        std::vector<std::string> _inputNames;
        std::unordered_map<std::string, InputNodeBase*> _inputNodeMap;

        std::vector<PortElementsBase> _outputElements;
        std::vector<std::string> _outputNames;
        std::unordered_map<std::string, PortElementsBase> _outputElementsMap;
    
        std::vector<const Node*> GetOutputNodes();
        void FixTransformedIO(ModelTransformer& transformer);
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
