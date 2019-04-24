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
#include <utilities/include/PropertyBag.h>

namespace ell
{
namespace model
{
    using namespace logging;

    MapCompiler::MapCompiler(const MapCompilerOptions& settings, const ModelOptimizerOptions& optimizerOptions) :
        _parameters(settings),
        _optimizerOptions(optimizerOptions)
    {
        PushScope();
    }

    MapCompilerOptions MapCompiler::GetMapCompilerOptions() const
    {
        return _parameters;
    }

    MapCompilerOptions MapCompiler::GetMapCompilerOptions(const Model& model) const
    {
        auto result = GetMapCompilerOptions();
        if (model.GetMetadata().HasEntry("compileOptions"))
        {
            return result.AppendOptions(model.GetMetadata().GetEntry<utilities::PropertyBag>("compileOptions"));
        }
        return result;
    }

    MapCompilerOptions MapCompiler::GetMapCompilerOptions(const Node& node) const
    {
        auto result = GetMapCompilerOptions(*node.GetModel());
        if (node.GetMetadata().HasEntry("compileOptions"))
        {
            return result.AppendOptions(node.GetMetadata().GetEntry<utilities::PropertyBag>("compileOptions"));
        }
        return result;
    }

    ModelOptimizerOptions MapCompiler::GetModelOptimizerOptions() const
    {
        return _optimizerOptions;
    }

    ModelOptimizerOptions MapCompiler::GetModelOptimizerOptions(const Model& model) const
    {
        ModelOptimizerOptions options = GetModelOptimizerOptions();
        if (model.GetMetadata().HasEntry("compileOptions"))
        {
            auto optionsMetadata = model.GetMetadata().GetEntry<utilities::PropertyBag>("compileOptions");
            AppendMetadataToOptions(optionsMetadata, options);
        }
        return options;
    }

    ModelOptimizerOptions MapCompiler::GetModelOptimizerOptions(const Node& node) const
    {
        ModelOptimizerOptions options = GetModelOptimizerOptions(*node.GetModel());
        if (node.GetMetadata().HasEntry("compileOptions"))
        {
            auto optionsMetadata = node.GetMetadata().GetEntry<utilities::PropertyBag>("compileOptions");
            AppendMetadataToOptions(optionsMetadata, options);
        }
        return options;
    }

    void MapCompiler::CompileMap(Map& map, const std::string& functionName)
    {
        using namespace std::string_literals;
        auto pModuleEmitter = GetModuleEmitter();

        Log() << "Creating 'predict' function" << EOL;
        emitters::NamedVariableTypeList mainFunctionArguments = AllocateMapFunctionArguments(map, *pModuleEmitter);
        pModuleEmitter->BeginMapPredictFunction(functionName, mainFunctionArguments);

        std::vector<std::string> comments;
        auto numInputs = map.NumInputs();
        for (size_t i = 0; i < numInputs; ++i)
        {
            comments.emplace_back("Input "s + std::to_string(i) + " ('" + map.GetInputName(i) + "') size: "s + std::to_string(map.GetInput(i)->Size()));
        }

        auto numOutputs = map.NumOutputs();
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
        std::unordered_set<const Node*> visitedNodes;
        model.Visit([this, &visitedNodes](const Node& node) {
            for (const auto* inputPort : node.GetInputPorts())
            {
                const auto* dependent = inputPort->GetReferencedPort().GetNode();
                if (visitedNodes.find(dependent) == visitedNodes.end())
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Visited node before all its descendants!");
                }
            }
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

            visitedNodes.insert(&node);
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
        for (auto output : map.GetOutputs())
        {
            // TODO: can we use an array type here?
            auto argVar = AllocatePortFunctionArgument(module, *output, ArgType::output, list);
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
} // namespace model
} // namespace ell
