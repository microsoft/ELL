////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "EmitterException.h"
#include "IRMapCompiler.h"
#include "MapCompiler.h"

// utilities
#include "UniqueId.h"
#include "Logger.h"

// stl
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <string>

namespace ell
{
namespace model
{
    using namespace logging;

    void CompilableNode::CompileNode(MapCompiler& compiler)
    {
        auto irCompiler = dynamic_cast<IRMapCompiler*>(&compiler);
        if (irCompiler == nullptr)
        {
            throw emitters::EmitterException(emitters::EmitterError::notSupported, "Unknown compiler type");
        }

        emitters::IRModuleEmitter& moduleEmitter = irCompiler->GetModule();
        auto& enclosingFunction = moduleEmitter.GetCurrentFunction();

        if (ShouldCompileInline() || compiler.GetMapCompilerOptions().inlineNodes)
        {
            Log() << "Inlining node " << DiagnosticString(*this) << " into function " << enclosingFunction.GetFunctionName() << EOL;

            irCompiler->NewNodeRegion(*this);
            Compile(*irCompiler, enclosingFunction);
            irCompiler->TryMergeNodeRegion(*this);
        }
        else
        {
            Log() << "Not inlining code for node " << DiagnosticString(*this) << EOL;

            // Emit code for function if it doesn't exist yet
            auto functionName = GetCompiledFunctionName();
            if (!moduleEmitter.HasFunction(functionName))
            {
                Log() << "Creating new function for " << DiagnosticString(*this) << EOL;

                compiler.PushScope();
                emitters::NamedVariableTypeList args = GetNodeFunctionParameterList(*irCompiler);

                // TODO: combine precompiled-IR case with use-own-function case
                if (HasPrecompiledIR())
                {
                    Log() << DiagnosticString(*this) << " has precompiled IR" << EOL;
                    auto functionCode = GetPrecompiledIR();
                    moduleEmitter.LoadIR(functionCode);
                }
                else if (HasOwnFunction())
                {
                    Log() << DiagnosticString(*this) << " has its own function" << EOL;
                    EmitNodeFunction(moduleEmitter);
                }
                else
                {
                    auto function = moduleEmitter.BeginFunction(functionName, emitters::VariableType::Void, args);
                    irCompiler->NewNodeRegion(*this);
                    Compile(*irCompiler, function);
                    irCompiler->TryMergeNodeRegion(*this);
                    moduleEmitter.EndFunction();
                }
                compiler.PopScope();
            }
            else
            {
                Log() << "Function " << functionName << " already exists for " << DiagnosticString(*this) << EOL;
            }

            // Call function for node
            irCompiler->NewNodeRegion(*this);
            CallNodeFunction(*irCompiler, enclosingFunction);
            irCompiler->TryMergeNodeRegion(*this);
        }
    }

    void CompilableNode::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    bool CompilableNode::ShouldCompileInline() const
    {
        // Make sure all inputs have only pure ports
        for (auto inputPort : GetInputPorts())
        {
            if (inputPort->GetInputElements().NumRanges() != 1)
            {
                return true;
            }

            // TODO: Re-enable compiling nodes with scalar inputs, once issues are worked out
            if (inputPort->GetInputElements().Size() == 1) // scalar
            {
                return true;
            }
        }
        return false;
    }

    std::string CompilableNode::GetCompiledFunctionName() const
    {
        auto baseName = _nodeFunctionPrefix + GetRuntimeTypeName();

        auto accumSizeString = [](const std::string& prefix, const Port* port) { return prefix + "_" + std::to_string(port->Size()); };
        auto inputPortStr = std::accumulate(GetInputPorts().begin(), GetInputPorts().end(), std::string("_in"), accumSizeString);
        auto outputPortStr = std::accumulate(GetOutputPorts().begin(), GetOutputPorts().end(), std::string("_out"), accumSizeString);
        auto stateId = GetInternalStateIdentifier();
        auto functionName = baseName + inputPortStr + outputPortStr;

        if (stateId != "")
        {
            functionName += std::string("_") + GetInternalStateIdentifier();
        }

        for (auto ch : _badIdentifierChars)
        {
            std::replace(functionName.begin(), functionName.end(), ch, '_');
        }

        return functionName;
    }

    bool CompilableNode::HasOwnFunction() const
    {
        return false;
    }

    void CompilableNode::EmitNodeFunction(emitters::IRModuleEmitter& module)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    bool CompilableNode::HasPrecompiledIR() const
    {
        return false;
    }

    std::string CompilableNode::GetPrecompiledIR() const
    {
        return "";
    }

    std::string CompilableNode::GetInternalStateIdentifier() const
    {
        if (HasState())
        {
            return IdString(*this);
        }
        else
        {
            return "";
        }
    }

    // Get parameters used in the node function's signature
    emitters::NamedVariableTypeList CompilableNode::GetNodeFunctionParameterList(IRMapCompiler& compiler) const
    {
        emitters::IRModuleEmitter& module = compiler.GetModule();
        emitters::NamedVariableTypeList args;
        for (auto port : GetInputPorts())
        {
            assert(port->GetInputElements().NumRanges() == 1); // if we're using an input port as a function argument, we need to preprocess model to ensure it's a full range
            auto varType = PortTypeToVariableType(port->GetType());
            auto ptrType = emitters::GetPointerType(varType);

            auto var = compiler.AllocateNodeFunctionArgument(module, port->GetInputElement(0), MapCompiler::ArgType::input);
            auto varName = var->EmittedName();
            args.emplace_back(varName, IsScalar(*port) ? varType : ptrType);
        };

        // add node state
        auto stateParams = GetNodeFunctionStateParameterList(compiler);
        for (const auto& param : stateParams)
        {
            args.emplace_back(param);
        }

        for (auto port : GetOutputPorts())
        {
            auto varType = PortTypeToVariableType(port->GetType());
            auto ptrType = emitters::GetPointerType(varType);

            auto var = compiler.AllocateNodeFunctionArgument(module, port, MapCompiler::ArgType::output);
            auto varName = var->EmittedName();
            args.emplace_back(varName, ptrType);
        };

        return args;
    }

    emitters::NamedVariableTypeList CompilableNode::GetNodeFunctionStateParameterList(IRMapCompiler& compiler) const
    {
        return {};
    }

    // Get the actual arguments use to call the node function
    std::vector<llvm::Value*> CompilableNode::GetNodeFunctionArguments(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const
    {
        int numArgs = GetInputPorts().size() + GetOutputPorts().size();
        std::vector<llvm::Value*> args;
        args.reserve(numArgs);

        for (auto port : GetInputPorts())
        {
            auto range = port->GetInputElements().GetRanges()[0];

            assert(port->GetInputElements().NumRanges() == 1); // if we're using an input port as a function argument, we need to preprocess model to ensure it's a full range
            auto inputArg = compiler.EnsurePortEmitted(*port);
            if (IsScalar(*port))
            {
                auto inputArgType = inputArg->getType();
                bool needsDereference = inputArgType->isPointerTy(); // This should perhaps be `isPtrOrPtrVectorTy()` or even `isPtrOrPtrVectorTy() || isArrayTy()`
                if (needsDereference)
                {
                    inputArg = currentFunction.ValueAt(inputArg, range.GetStartIndex());
                }

                args.push_back(inputArg);
            }
            else
            {
                auto index = port->GetInputElements().GetRanges()[0].GetStartIndex();
                auto inputArgPtr = currentFunction.PointerOffset(inputArg, index);
                args.push_back(inputArgPtr);
            }
        };

        // add node state
        auto stateArgs = GetNodeFunctionStateArguments(compiler, currentFunction);
        for (const auto& arg : stateArgs)
        {
            args.emplace_back(arg);
        }

        for (auto port : GetOutputPorts())
        {
            auto outputArg = compiler.EnsurePortEmitted(*port);
            auto outputArgPtr = currentFunction.PointerOffset(outputArg, 0);
            args.push_back(outputArgPtr);
        };

        return args;
    }

    std::vector<llvm::Value*> CompilableNode::GetNodeFunctionStateArguments(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const
    {
        return {};
    }

    void CompilableNode::CallNodeFunction(IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction)
    {
        auto functionName = GetCompiledFunctionName();
        auto function = compiler.GetModule().GetFunction(functionName);
        assert(function != nullptr);
        auto args = GetNodeFunctionArguments(compiler, currentFunction);
        currentFunction.Call(function, args);
        Log() << "Emitting call to node function " << functionName << EOL;
    }
}
}
