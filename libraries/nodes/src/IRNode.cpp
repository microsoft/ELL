////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRNode.h"

// model
#include "CompilableNodeUtilities.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    IRNode::IRNode(const std::vector<model::InputPortBase*>& inputPorts, const std::vector<model::OutputPortBase*>& outputPorts, const std::string& functionName, const std::string& irCode)
        : IRNode(inputPorts, outputPorts, functionName, irCode, {})
    {
    }

    IRNode::IRNode(const std::vector<model::InputPortBase*>& inputPorts, const std::vector<model::OutputPortBase*>& outputPorts, const std::string& functionName, const std::string& irCode, const emitters::NamedVariableTypeList& extraArgs)
        : CompilableNode(inputPorts, outputPorts), _functionName(functionName), _irCode(irCode), _extraArgs(extraArgs)
    {
    }

    emitters::NamedVariableTypeList IRNode::GetInputTypes()
    {
        emitters::NamedVariableTypeList types;
        for (auto inputPort : GetInputPorts())
        {
            types.emplace_back(inputPort->GetName(), model::GetPortVariableType(*inputPort));
        }
        return types;
    }

    emitters::NamedVariableTypeList IRNode::GetOutputTypes()
    {
        emitters::NamedVariableTypeList types;
        for (auto outputPort : GetOutputPorts())
        {
            types.emplace_back(outputPort->GetName(), model::GetPortVariableType(*outputPort));
        }
        return types;
    }

    void IRNode::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "IRNode::Compute not implemented");
    };

    void IRNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);

        Node::WriteToArchive(archiver);
        archiver["name"] << _functionName;
        archiver["irCode"] << _irCode;
        // TODO: inputTypes, outputTypes, extraArgs
    }

    void IRNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);

        Node::ReadFromArchive(archiver);
        archiver["name"] >> _functionName;
        archiver["irCode"] >> _irCode;
        // TODO: inputTypes, outputTypes, extraArgs
    }

    std::string IRNode::GetCompiledFunctionName() const
    {
        return _functionName;
    }

    emitters::NamedVariableTypeList IRNode::GetNodeFunctionStateParameterList(model::IRMapCompiler& compiler) const
    {
        return _extraArgs;
    }

    std::vector<llvm::Value*> IRNode::GetNodeFunctionStateArguments(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const
    {
        assert(_extraArgs.size() == 0); // currently doesn't work if you have 'other' args
        return {};
    }

    bool IRNode::HasPrecompiledIR() const
    {
        return true;
    }

    std::string IRNode::GetPrecompiledIR() const
    {
        return _irCode;
    }
}
}
