////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SourceNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "InputNodeBase.h"
#include "IRMapCompiler.h"
#include "ModelTransformer.h"

// nodes
#include "ClockNode.h"

// emitters
#include "IRMetadata.h"
#include "IRModuleEmitter.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A function that the SourceNode calls to receive data from user code. </summary>
    ///
    /// In device-side compiled code, the function signature should be:
    /// ```
    /// bool SourceFunction(ValueType* data)
    /// ```
    /// where the bool return value is used to indicate if a new sample is available from the source
    template <typename ValueType>
    using SourceFunction = std::function<bool(std::vector<ValueType>&)>;

    /// <summary> A node that provides a source of data through a sampling function callback. </summary>
    template <typename ValueType>
    class SourceNode : public model::SourceNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<TimeTickType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        SourceNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time). </param>
        /// <param name="shape"> The input shape. </param>
        /// <param name="sourceFunctionName"> The source function name to be emitted. </param>
        /// <param name="source"> The optional source function that will provide input values. </param>
        SourceNode(const model::PortElements<nodes::TimeTickType>& input, const math::TensorShape& shape, const std::string& sourceFunctionName, SourceFunction<ValueType> source = nullptr);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time). </param>
        /// <param name="inputVectorSize"> The input vector size. </param>
        /// <param name="sourceFunctionName"> The source function name to be emitted. </param>
        /// <param name="source"> The optional source function that will provide input values. </param>
        SourceNode(const model::PortElements<nodes::TimeTickType>& input, size_t inputVectorSize, const std::string& sourceFunctionName, SourceFunction<ValueType> source = nullptr);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SourceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer. </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` receiving the copy. </param>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Interpolates the buffered sample to match the new time. </summary>
        ///
        /// <param name="originalTime"> Original time for the buffered sample. </param>
        /// <param name="newTime"> New time for the buffered sample. </param>
        virtual void Interpolate(TimeTickType originalTime, TimeTickType newTime) const;

        /// <summary> Sets the source function for this node for use in Compute(). </summary>
        ///
        /// <param name="function"> The source function to set. </param>
        void SetSourceFunction(SourceFunction<ValueType> function) { _source = function; }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: callback function name, shape

    private:
        void SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* sample);
        void SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* sample);

    private:
        model::InputPort<TimeTickType> _input;
        model::OutputPort<ValueType> _output;

        SourceFunction<ValueType> _source;
        
        mutable std::vector<ValueType> _bufferedSample;
        mutable TimeTickType _bufferedSampleTime;
    };
}
}

#include "../tcc/SourceNode.tcc"