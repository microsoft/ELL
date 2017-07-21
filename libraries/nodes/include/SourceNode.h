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
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "SteppableMap.h"

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
    // CONSIDER switching from a template parameter to an std::function member, so that the sampling
    // function can be overwritten post-unachiving (the TypeFactory currently sets a no-op function).
    template <typename ValueType>
    using SamplingFunction = bool (*)(std::vector<ValueType>&);
    using TimeTickType = model::TimeTickType;

    /// <summary> A node that provides a source of data through a sampling function callback. </summary>
    template <typename ValueType, SamplingFunction<ValueType>>
    class SourceNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<TimeTickType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor </summary>
        SourceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time) </param>
        /// <param name="outputSize"> Output size </param>
        SourceNode(const model::PortElements<TimeTickType>& input, size_t outputSize);

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> Port elements for input values (sample time, current time) </param>
        /// <param name="outputSize"> Output size </param>
        /// <param name="samplingFunctionName"> Sampling function name to be emitted (defaults to "SourceNode_SamplingFunction") </param>
        SourceNode(const model::PortElements<TimeTickType>& input, size_t outputSize, const std::string& samplingFunctionName);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SourceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Interpolates the buffered sample to match the new time. </summary>
        ///
        /// <param name="originalTime"> Original time for the buffered sample </param>
        /// <param name="newTime"> New time for the buffered sample </param>
        virtual void Interpolate(TimeTickType originalTime, TimeTickType newTime) const;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* sample);
        void SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* sample);

    private:
        model::InputPort<TimeTickType> _input;
        model::OutputPort<ValueType> _output;
        std::string _samplingFunctionName;

        mutable std::vector<ValueType> _bufferedSample;
        mutable TimeTickType _bufferedSampleTime;
    };
}
}

#include "../tcc/SourceNode.tcc"