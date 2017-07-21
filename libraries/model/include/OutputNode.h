////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

// utilities
#include "IArchivable.h"

// stl
#include <memory>
#include <string>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    /// <summary> A node that represents an output from the system. </summary>
    class OutputNodeBase : public CompilableNode
    {
    public:
        const InputPortBase& GetInputPort() const { return _inputBase; }
        const OutputPortBase& GetOutputPort() const { return _outputBase; }
        using Node::GetInputPort;
        using Node::GetOutputPort;

    protected:
        OutputNodeBase(InputPortBase& input, OutputPortBase& output);
        virtual bool ShouldCompileInline() const override { return true; }

    private:
        InputPortBase& _inputBase;
        OutputPortBase& _outputBase;
    };

    /// <summary> A node that represents an output from the system. </summary>
    template <typename ValueType>
    class OutputNode : public OutputNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        OutputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The `PortElements<>` to get the input data from </param>
        OutputNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    protected:
        virtual void Compute() const override;
        virtual void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;
    };
}
}

#include "../tcc/OutputNode.tcc"
