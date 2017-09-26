////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DebugSinkNode.h (nodes)
//  Authors:  Chris Lovett
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

// stl
#include <functional>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A function that the DebugSinkNode calls to deliver data to user code. </summary>
    ///
    /// In device-side compiled code, the function signature should be:
    /// ```
    /// void DebugSinkFunction(char* label, ValueType* input, void* userData)
    /// ```
    template<typename ValueType>
    using DebugSinkFunction = std::function<void(const std::string&, const std::vector<ValueType>&, void* userData)>;

    template<typename ValueType>
    class DebugSinkNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor </summary>
        DebugSinkNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> Port elements for input values </param>
        /// <param name="sink"> The sink function that will receive output values </param>
        /// <param name="label"> The optional label to be passed to the DebugSinkFunction </param>
        /// <param name="sinkFunctionName"> The optional sink function name to be emitted </param>
        DebugSinkNode(const model::PortElements<ValueType>& input, DebugSinkFunction<ValueType> sink, const std::string& label, void* userData, const std::string& sinkFunctionName);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DebugSinkNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` receiving the copy  </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary></summary>
        virtual void ChangeSink(DebugSinkFunction<ValueType> sink) { _sink = sink; } // STYLE violation

        /// <summary></summary>
        virtual std::string GetLabel() const { return _label; }

    protected:
        virtual bool ShouldCompileInline() const override;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // Evaluates whether the input meets the filter criteria,
        // and should be forwarded to the sink function.
        virtual bool EvaluateInput() const;

    private:
        model::InputPort<ValueType> _input;
        model::OutputPort<ValueType> _output;
        std::string _label;
        void* _userData;
        std::string _sinkFunctionName;
        DebugSinkFunction<ValueType> _sink;
    };
}
}

#include "../tcc/DebugSinkNode.tcc"