////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "TypeName.h"

// llvm
#include <llvm/IR/Value.h>

// stl
#include <initializer_list>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary>
    /// A base class for nodes that emit pregenerated LLVM IR code
    ///
    /// In order to create a new concrete IR node, subclass `IRNode` and implement the following:
    /// - constructor -- must call protected constructor to set up mapping to real ports
    /// - Copy method
    /// - if the node has extra arguments, need to implement GetNodeFunctionStateArguments()
    ///
    /// </summary>
    class IRNode : public model::CompilableNode
    {
    public:
        IRNode() = default;

        IRNode(const IRNode&) = delete;
        IRNode(IRNode&&) = delete;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IRNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently copying the model </param>
        // virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary></summary>
        std::string GetFunctionName() const { return _functionName; }

        /// <summary></summary>
        std::string GetIRCode() const { return _irCode; }

    protected:
        /// <summary> Constructor </summary>
        ///
        /// <param name="inputPorts"> Pointers to the real input ports in the concrete node subclass </param>
        /// <param name="outputPorts"> Pointers to the real output ports in the concrete node subclass </param>
        /// <param name="functionName"> The name of the function implemented by the IR </param>
        /// <param name="irCode"> LLVM IR code that implements the node's compute function </param>
        /// <param name="extraArgs"> A set of name/type pairs for any extra function arguments </param>
        IRNode(const std::vector<model::InputPortBase*>& inputPorts, const std::vector<model::OutputPortBase*>& outputPorts, const std::string& functionName, const std::string& irCode);
        IRNode(const std::vector<model::InputPortBase*>& inputPorts, const std::vector<model::OutputPortBase*>& outputPorts, const std::string& functionName, const std::string& irCode, const emitters::NamedVariableTypeList& extraArgs);

        void Compute() const override;

        bool HasPrecompiledIR() const override;
        std::string GetPrecompiledIR() const override;

        std::string GetCompiledFunctionName() const override;
        emitters::NamedVariableTypeList GetNodeFunctionStateParameterList(model::IRMapCompiler& compiler) const override;
        std::vector<llvm::Value*> GetNodeFunctionStateArguments(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const override;

        emitters::NamedVariableTypeList GetInputTypes();
        emitters::NamedVariableTypeList GetOutputTypes();
        const emitters::NamedVariableTypeList& GetExtraArgs() const { return _extraArgs; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        std::string _functionName;
        std::string _irCode;
        emitters::NamedVariableTypeList _extraArgs;

        // we maybe don't even need these (?)
        emitters::NamedVariableTypeList _inputTypes;
        emitters::NamedVariableTypeList _outputTypes;
    };
}
}
