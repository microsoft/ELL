////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompiler.cpp (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapCompiler.h"
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h" // for PortTypeToVariableType
#include "EmitterException.h"
#include "Map.h"
#include "Model.h"
#include "Node.h"

// utilities
#include "Logger.h"

namespace ell
{
namespace model
{
    using namespace logging;

    MapCompiler::MapCompiler(const MapCompilerOptions& settings)
        : _parameters(settings)
    {
        PushScope();
    }

    void MapCompiler::CompileMap(Map& map, const std::string& functionName)
    {
        auto pModuleEmitter = GetModuleEmitter();

        emitters::NamedVariableTypeList mainFunctionArguments = AllocateNodeFunctionArguments(map, *pModuleEmitter);
        pModuleEmitter->BeginMapPredictFunction(functionName, mainFunctionArguments);

        Log() << "Creating 'predict' function" << EOL;
        auto inputSize = map.GetInput(0)->Size();
        auto outputSize = map.GetOutput(0).Size();
        std::vector<std::string> comments = { std::string("Input size: ") + std::to_string(inputSize), std::string("Output size: ") + std::to_string(outputSize) };
        pModuleEmitter->SetFunctionComments(functionName, comments);

        OnBeginCompileModel(map.GetModel());
        CompileNodes(map.GetModel());
        OnEndCompileModel(map.GetModel());

        pModuleEmitter->EndMapPredictFunction();
        Log() << "Finished 'predict' function" << EOL;
    }

    void MapCompiler::CompileNodes(Model& model)
    {
        model.Visit([this](const Node& node) {
            if (!node.IsCompilable(this))
            {
                std::string typeName = node.GetRuntimeTypeName();
                throw emitters::EmitterException(emitters::EmitterError::notSupported, std::string("Uncompilable node type: " + typeName));
            }

            auto compilableNode = const_cast<CompilableNode*>(dynamic_cast<const CompilableNode*>(&node));
            assert(compilableNode != nullptr && "Got null compilable node");

            Log() << "Now compiling node " << DiagnosticString(node) << EOL;
            OnBeginCompileNode(node);
            compilableNode->CompileNode(*this);
            OnEndCompileNode(node);
        });
    }

    emitters::Variable* MapCompiler::AllocatePortVariable(const OutputPortBase& port)
    {
        auto pModuleEmitter = GetModuleEmitter();
        assert(port.Size() != 0);

        emitters::VariableType varType = PortTypeToVariableType(port.GetType());
        emitters::Variable* pVar = nullptr;
        bool isScalar = port.Size() == 1;
        if (isScalar) // TODO: only do this if scope != output (or, only if scope == input or local?)
        {
            pVar = pModuleEmitter->Variables().AddScalarVariable(emitters::VariableScope::local, varType);
        }
        else
        {
            pVar = pModuleEmitter->Variables().AddVectorVariable(emitters::VariableScope::global, varType, port.Size());
        }

        pModuleEmitter->AllocateVariable(*pVar);
        SetVariableForPort(port, pVar);
        return pVar;
    }

    emitters::Variable* MapCompiler::GetOrAllocatePortVariable(const OutputPortBase& port)
    {
        emitters::Variable* pVar = GetVariableForPort(port);
        if (pVar == nullptr)
        {
            pVar = AllocatePortVariable(port);
        }
        assert(pVar != nullptr);
        return pVar;
    }

    //
    // Allocating variables for function arguments
    //
    emitters::NamedVariableTypeList MapCompiler::AllocateNodeFunctionArguments(Map& map, emitters::ModuleEmitter& module)
    {
        emitters::NamedVariableTypeList functionArguments;

        // Allocate variables for inputs
        for (auto inputNode : map.GetInputs())
        {
            auto argVar = AllocateNodeFunctionArgument(module, &(inputNode->GetOutputPort()), ArgType::input);
            auto inputSize = inputNode->Size();
            bool isScalar = inputSize == 1;
            if (isScalar)
            {
                functionArguments.push_back({ argVar->EmittedName(), argVar->Type() });
            }
            else
            {
                // TODO: can we use an array type here?
                functionArguments.push_back({ argVar->EmittedName(), GetPointerType(argVar->Type()) });
            }
        }

        // Allocate variables for outputs -- scalar outputs treated the same as vector
        for (auto outputElements : map.GetOutputs())
        {
            assert(outputElements.NumRanges() == 1);

            // TODO: can we use an array type here?
            auto argVar = AllocateNodeFunctionArgument(module, outputElements.GetRanges()[0].ReferencedPort(), ArgType::output);
            functionArguments.push_back({ argVar->EmittedName(), GetPointerType(argVar->Type()) });
        }
        return functionArguments;
    }

    emitters::Variable* MapCompiler::AllocateNodeFunctionArgument(emitters::ModuleEmitter& module, const OutputPortBase* pPort, ArgType argType)
    {
        assert(pPort->Size() != 0);
        bool isScalar = pPort->Size() == 1;
        emitters::VariableType varType = PortTypeToVariableType(pPort->GetType());
        emitters::VariableScope scope = argType == ArgType::input ? emitters::VariableScope::input : emitters::VariableScope::output;

        // outputs are modelled as Vectors
        emitters::Variable* pVar = nullptr;
        if (isScalar && argType == ArgType::input)
        {
            pVar = module.Variables().AddScalarVariable(scope, varType);
        }
        else
        {
            pVar = module.Variables().AddVectorVariable(scope, varType, pPort->Size());
        }
        module.AllocateVariable(*pVar);
        SetVariableForPort(*pPort, pVar);
        return pVar;
    }

    emitters::Variable* MapCompiler::AllocateNodeFunctionArgument(emitters::ModuleEmitter& module, const PortElementBase& element, ArgType argType)
    {
        // TODO: fix this to return a VectorElementVariable
        return AllocateNodeFunctionArgument(module, element.ReferencedPort(), argType);
    }

    void MapCompiler::PushScope()
    {
        Log() << "Compiler creating new scope" << EOL;
        _portToVarMaps.emplace_back();
    }

    void MapCompiler::PopScope()
    {
        Log() << "Compiler popping scope" << EOL;
        assert(_portToVarMaps.size() > 0);
        _portToVarMaps.pop_back();
    }

    emitters::Variable* MapCompiler::GetVariableForElement(const PortElementBase& element)
    {
        return GetVariableForPort(*element.ReferencedPort());
    }

    emitters::Variable* MapCompiler::GetVariableForPort(const OutputPortBase& port)
    {
        assert(_portToVarMaps.size() > 0);
        auto search = _portToVarMaps.back().find(&port);
        if (search != _portToVarMaps.back().end())
        {
            return search->second;
        }
        return nullptr;
    }

    void MapCompiler::SetVariableForPort(const Port& port, emitters::Variable* pVar)
    {
        _portToVarMaps.back()[&port] = pVar;
    }

    void MapCompiler::SetVariableForElement(const PortElementBase& element, emitters::Variable* pVar)
    {
        _portToVarMaps.back()[element.ReferencedPort()] = pVar;
    }
}
}
