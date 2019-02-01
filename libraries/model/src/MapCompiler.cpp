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
#include "Map.h"
#include "Model.h"
#include "Node.h"

#include <emitters/include/EmitterException.h>

#include <utilities/include/Logger.h>

namespace ell
{
namespace model
{
    using namespace logging;

    MapCompiler::MapCompiler(const MapCompilerOptions& settings) :
        _parameters(settings)
    {
        PushScope();
    }

    void MapCompiler::CompileMap(Map& map, const std::string& functionName)
    {
        using namespace std::string_literals;
        auto pModuleEmitter = GetModuleEmitter();

        Log() << "Creating 'predict' function" << EOL;
        emitters::NamedVariableTypeList mainFunctionArguments = AllocateMapFunctionArguments(map, *pModuleEmitter);
        pModuleEmitter->BeginMapPredictFunction(functionName, mainFunctionArguments);

        std::vector<std::string> comments;
        auto numInputs = map.GetNumInputs();
        for (size_t i = 0; i < numInputs; ++i)
        {
            comments.emplace_back("Input "s + std::to_string(i) + " ('" + map.GetInputName(i) + "') size: "s + std::to_string(map.GetInput(i)->Size()));
        }

        auto numOutputs = map.GetNumOutputs();
        for (size_t i = 0; i < numOutputs; ++i)
        {
            comments.emplace_back("Output "s + std::to_string(i) + " ('" + map.GetOutputName(i) + "') size: "s + std::to_string(map.GetOutput(i).Size()));
        }

        pModuleEmitter->GetFunctionDeclaration(functionName).GetComments() = comments;

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
            if (!compilableNode)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Encountered null compilable node");
            }

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
        auto pVar = pModuleEmitter->Variables().AddVectorVariable(emitters::VariableScope::global, varType, port.Size());
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
    emitters::NamedVariableTypeList MapCompiler::AllocateMapFunctionArguments(Map& map, emitters::ModuleEmitter& module)
    {
        emitters::NamedVariableTypeList functionArguments;

        // context parameter
        functionArguments.push_back({ "context", emitters::VariableType::VoidPointer });

        utilities::UniqueNameList list;
        // Allocate variables for inputs
        for (auto inputNode : map.GetInputs())
        {
            auto argVar = AllocatePortFunctionArgument(module, inputNode->GetOutputPort(), ArgType::input, list);
            functionArguments.push_back({ argVar->EmittedName(), GetPointerType(argVar->Type()) });
        }

        // Allocate variables for outputs -- scalar outputs are treated the same as vectors
        for (auto outputElements : map.GetOutputs())
        {
            assert(outputElements.NumRanges() == 1);

            // TODO: can we use an array type here?
            auto argVar = AllocatePortFunctionArgument(module, *outputElements.GetRanges()[0].ReferencedPort(), ArgType::output, list);
            functionArguments.push_back({ argVar->EmittedName(), GetPointerType(argVar->Type()) });
        }
        return functionArguments;
    }

    emitters::Variable* MapCompiler::AllocatePortFunctionArgument(emitters::ModuleEmitter& module, const OutputPortBase& port, ArgType argType, ell::utilities::UniqueNameList& list)
    {
        emitters::VariableType varType = PortTypeToVariableType(port.GetType());
        emitters::VariableScope scope = argType == ArgType::input ? emitters::VariableScope::input : emitters::VariableScope::output;

        // outputs are modelled as Vectors
        emitters::Variable* pVar = module.Variables().AddVectorVariable(scope, varType, port.Size());

        std::string defaultName = argType == ArgType::input ? "input" : "output";
        std::string friendlyName = list.Add(port.GetVariableName(defaultName));
        pVar->SetEmittedName(friendlyName);
        
        module.AllocateVariable(*pVar);
        SetVariableForPort(port, pVar);
        return pVar;
    }

    emitters::Variable* MapCompiler::AllocatePortFunctionArgument(emitters::ModuleEmitter& module, const PortElementBase& element, ArgType argType, ell::utilities::UniqueNameList& list)
    {
        return AllocatePortFunctionArgument(module, *element.ReferencedPort(), argType, list);
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
        SetVariableForPort(*element.ReferencedPort(), pVar);
    }
} // namespace model
} // namespace ell
