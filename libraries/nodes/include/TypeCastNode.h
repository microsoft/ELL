////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeCastNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

// stl
#include <string>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> A node that represents an output from the system. </summary>
    template <typename InputValueType, typename OutputValueType>
    class TypeCastNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<InputValueType>& input = _input;
        const model::OutputPort<OutputValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        TypeCastNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        TypeCastNode(const model::PortElements<InputValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputValueType, OutputValueType>("TypeCastNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool ShouldCompileInline() const override { return true; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

        void CompileLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        model::InputPort<InputValueType> _input;
        model::OutputPort<OutputValueType> _output;
    };
}
}

#include "../tcc/TypeCastNode.tcc"
