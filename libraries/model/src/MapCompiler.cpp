////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompiler.cpp (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapCompiler.h"
#include "CompilableNodeUtilities.h" // PortTypeToVariableType
#include "EmitterException.h"
#include "CompilableNode.h"

namespace ell
{
namespace model
{
    void MapCompiler::CompileMap(model::DynamicMap& map, const std::string& functionName)
    {
        auto pModuleEmitter = GetModuleEmitter();
        ClearArgs();

        // Allocate variables for inputs
        for (auto inputNode : map.GetInputs())
        {
            AllocArg(*pModuleEmitter, &(inputNode->GetOutputPort()), ArgType::input);
        }

        // Allocate variables for outputs
        for (auto outputElements : map.GetOutputs())
        {
            assert(outputElements.NumRanges() == 1);
            AllocArg(*pModuleEmitter, outputElements.GetRanges()[0].ReferencedPort(), ArgType::output);
        }

        pModuleEmitter->BeginFunction(functionName, _arguments);
        CompileNodes(map.GetModel());
        pModuleEmitter->EndFunction();
    }

    void MapCompiler::CompileNodes(model::Model& model)
    {
        model.Visit([this](const model::Node& node) {
            if (!node.IsCompilable())
            {
                std::string typeName = node.GetRuntimeTypeName();
                throw emitters::EmitterException(emitters::EmitterError::notSupported, std::string("Unknown node type: " + typeName));
            }

            OnBeginCompileNode(node);
            auto compilableNode = const_cast<CompilableNode*>(dynamic_cast<const CompilableNode*>(&node));
            assert(compilableNode != nullptr && "Got null compilable node");
            compilableNode->Compile(*this);
            OnEndCompileNode(node);
        });
    }

    emitters::Variable* MapCompiler::AllocatePortVariable(model::OutputPortBase* pPort)
    {
        auto pModuleEmitter = GetModuleEmitter();
        assert(pPort->Size() != 0);

        emitters::VariableType varType = PortTypeToVariableType(pPort->GetType());
        emitters::Variable* pVar = nullptr;
        if (pPort->Size() == 1)
        {
            pVar = pModuleEmitter->Variables().AddScalarVariable(emitters::VariableScope::local, varType);
        }
        else
        {
            pVar = pModuleEmitter->Variables().AddVectorVariable(emitters::VariableScope::global, varType, pPort->Size());
        }
        SetVariableFor(pPort, pVar);
        return pVar;
    }

    emitters::Variable* MapCompiler::GetOrAllocatePortVariable(model::OutputPortBase* pPort)
    {
        assert(pPort != nullptr);
        emitters::Variable* pVar = GetVariableFor(pPort);
        if (pVar == nullptr)
        {
            pVar = AllocatePortVariable(pPort);
        }
        assert(pVar != nullptr);
        return pVar;
    }

    emitters::Variable* MapCompiler::AllocArg(emitters::ModuleEmitter& emitter, const model::OutputPortBase* pPort, ArgType argType)
    {
        assert(pPort->Size() != 0);

        emitters::VariableType varType = PortTypeToVariableType(pPort->GetType());
        emitters::VariableScope scope = argType == ArgType::input ? emitters::VariableScope::input : emitters::VariableScope::output;

        //
        // For now, all inputs and outputs are modelled as Vectors... unlike regular variables, we don't optimize for scalars
        //
        emitters::Variable* pVar = emitter.Variables().AddVectorVariable(scope, varType, pPort->Size());
        emitter.AllocateVariable(*pVar);
        SetVariableFor(pPort, pVar);

        _arguments.Append({ pVar->EmittedName(), GetPointerType(varType) });
        if (argType == ArgType::input)
        {
            _inputArgs.Append({ pVar->EmittedName(), GetPointerType(varType) });
        }

        return pVar;
    }

    void MapCompiler::ClearArgs()
    {
        // Can we free the variables associated with the args?
        _arguments.Clear();
        _inputArgs.Clear();
    }

    emitters::ModuleEmitter* MapCompiler::GetModuleEmitter()
    {
        auto pModuleEmitter = dynamic_cast<emitters::ModuleEmitter*>(this);
        if (pModuleEmitter == nullptr)
        {
            throw emitters::EmitterException(emitters::EmitterError::notSupported, "Unknown Compiler class");
        }
        return pModuleEmitter;
    }

    emitters::Variable* MapCompiler::GetVariableFor(const model::OutputPortBase* pPort)
    {
        assert(pPort != nullptr);
        auto search = _portToVarMap.find(pPort);
        if (search != _portToVarMap.end())
        {
            return search->second;
        }
        return nullptr;
    }

    emitters::Variable* MapCompiler::EnsureVariableFor(const model::OutputPortBase* pPort)
    {
        emitters::Variable* pVar = GetVariableFor(pPort);
        if (pVar == nullptr)
        {
            throw emitters::EmitterException(emitters::EmitterError::notSupported, "Variable for output port not found");
        }
        return pVar;
    }

    emitters::Variable* MapCompiler::GetVariableFor(const model::PortElementBase& element)
    {
        return GetVariableFor(element.ReferencedPort());
    }

    emitters::Variable* MapCompiler::EnsureVariableFor(const model::PortElementBase& element)
    {
        return EnsureVariableFor(element.ReferencedPort());
    }

    void MapCompiler::SetVariableFor(const model::OutputPortBase* pPort, emitters::Variable* pVar)
    {
        _portToVarMap[pPort] = pVar;
    }
}
}
