////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRCompiledMap.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRCompiledMap.h"
#include "CompilableNodeUtilities.h"
#include "EmitterException.h"
#include "IRMapCompiler.h"

// model
#include "ModelTransformer.h"
#include "OutputNode.h"
#include "Port.h"

// utilities
#include "Exception.h"
#include "Files.h"

// stl
#include <sstream>

namespace ell
{
namespace model
{
    IRCompiledMap::IRCompiledMap(const model::DynamicMap& other, const std::string& functionName, bool optimize)
        : CompiledMap(other, functionName, optimize)
    {
        Compile();
    }

    IRCompiledMap::IRCompiledMap(IRCompiledMap&& other)
        : CompiledMap(std::move(other)), _moduleName(std::move(other._moduleName)), _module(std::move(other._module)), _executionEngine(std::move(other._executionEngine))
    {
        // We need to re-extract the compute function address -- the default move constructor doesn't do that correctly for some reason
        SetComputeFunction();
    }

    void IRCompiledMap::Compile()
    {
        // Make sure the compilable node registry is initialized
        // InitCompilableNodeRegistry();

        EnsureValidMap();

        // model::TransformContext context{ [](const model::Node& node) { return node.IsCompilable() || CanMakeCompilable(node) ? model::NodeAction::compile : model::NodeAction::refine; } };
        model::TransformContext context{ [](const model::Node& node) { return node.IsCompilable() ? model::NodeAction::compile : model::NodeAction::refine; } };
        Refine(context);

        // Now transform nodes into compilable nodes
        // Transform(TryMakeCompilableNode, context);

        // Now we have the map, compile it
        emitters::CompilerParameters settings;
        settings.optimize = _optimize;
        settings.includeDiagnosticInfo = false;

        IRMapCompiler compiler(_moduleName);
        compiler.SetCompilerParameters(settings);
        compiler.CompileMap(*this, _functionName);
        _module = std::make_unique<emitters::IRModuleEmitter>(compiler.TransferOwnership());

        // Instead of recompiling, can we just clone the module and jit it?
        IRMapCompiler jitCompiler(_moduleName + "_jit");
        jitCompiler.SetCompilerParameters(settings);
        jitCompiler.CompileMap(*this, _functionName);
        _executionEngine = jitCompiler.Jit();
        SetComputeFunction(); // extract the compute function from the execution engine
    }

    bool IRCompiledMap::IsValid() const
    {
        return _module != nullptr && _module->IsValid();
    }

    void IRCompiledMap::SetComputeFunction()
    {
        switch (GetInput(0)->GetOutputPort().GetType())
        {
            case model::Port::PortType::boolean:
                SetComputeFunctionForInputType<bool>();
                break;

            case model::Port::PortType::integer:
                SetComputeFunctionForInputType<int>();
                break;

            case model::Port::PortType::real:
                SetComputeFunctionForInputType<double>();
                break;

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    void IRCompiledMap::SetNodeInput(model::InputNode<bool>* node, const std::vector<bool>& inputValues) const
    {
        if (GetInput(0)->GetOutputPort().GetType() != node->GetOutputPort().GetType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
        utilities::ConformingVector<bool> temp(inputValues.size());
        for (size_t index = 0; index < temp.size(); ++index)
        {
            temp[index] = static_cast<bool>(inputValues[index]);
        }

        std::get<ComputeFunction<bool>>(_computeInputFunction)((bool*)temp.data());
    }

    void IRCompiledMap::SetNodeInput(model::InputNode<int>* node, const std::vector<int>& inputValues) const
    {
        if (GetInput(0)->GetOutputPort().GetType() != node->GetOutputPort().GetType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        std::get<ComputeFunction<int>>(_computeInputFunction)(inputValues.data());
    }

    void IRCompiledMap::SetNodeInput(model::InputNode<double>* node, const std::vector<double>& inputValues) const
    {
        if (GetInput(0)->GetOutputPort().GetType() != node->GetOutputPort().GetType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        std::get<ComputeFunction<double>>(_computeInputFunction)(inputValues.data());
    }

    std::vector<bool> IRCompiledMap::ComputeBoolOutput(const model::PortElementsBase& outputs) const
    {
        if (GetOutput(0).GetPortType() != model::Port::PortType::boolean)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        // Terrible hack to create a std::vector<bool>
        return std::vector<bool>((bool*)(std::get<utilities::ConformingVector<bool>>(_cachedOutput).data()), (bool*)(std::get<utilities::ConformingVector<bool>>(_cachedOutput).data() + std::get<utilities::ConformingVector<bool>>(_cachedOutput).size()));
    }

    std::vector<int> IRCompiledMap::ComputeIntOutput(const model::PortElementsBase& outputs) const
    {
        if (GetOutput(0).GetPortType() != model::Port::PortType::integer)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        return std::get<utilities::ConformingVector<int>>(_cachedOutput);
    }

    std::vector<double> IRCompiledMap::ComputeDoubleOutput(const model::PortElementsBase& outputs) const
    {
        if (GetOutput(0).GetPortType() != model::Port::PortType::real)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        return std::get<utilities::ConformingVector<double>>(_cachedOutput);
    }

    void IRCompiledMap::WriteCode(const std::string& filePath)
    {
        _module->WriteToFile(filePath);
    }

    void IRCompiledMap::WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format)
    {
        _module->WriteToFile(filePath, format);
    }

    void IRCompiledMap::WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format, emitters::MachineCodeOutputOptions options)
    {
        _module->WriteToFile(filePath, format, options);
    }

    void IRCompiledMap::WriteCodeHeader(const std::string& filePath) const
    {
        auto stream = utilities::OpenOfstream(filePath);
        WriteCodeHeader(stream);
    }

    void IRCompiledMap::WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format)
    {
        _module->WriteToStream(stream, format);
    }

    void IRCompiledMap::WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format, emitters::MachineCodeOutputOptions options)
    {
        _module->WriteToStream(stream, format, options);
    }

    void IRCompiledMap::WriteCodeHeader(std::ostream& stream) const
    {
        auto inputSize = GetInput(0)->Size();
        auto inputType = GetInput(0)->GetOutputType();
        auto outputSize = GetOutput(0).Size();
        auto outputType = GetOutput(0).GetPortType();

        stream << "extern \"C\" void " << _functionName << "(";
        stream << GetPortCTypeName(inputType) << " input[" << inputSize << "], ";
        stream << GetPortCTypeName(outputType) << " output[" << outputSize << "]);";
    }

    std::string IRCompiledMap::GetCodeHeaderString() const
    {
        std::stringstream s;
        WriteCodeHeader(s);
        return s.str();
    }

    void IRCompiledMap::EnsureValidMap()
    {
        if (NumInputPorts() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Compiled maps must have a single input");
        }

        if (NumOutputPorts() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Compiled maps must have a single output");
        }

        // if output isn't a simple port, add an output node to model
        auto out = GetOutput(0);
        if (!out.IsFullPortOutput())
        {
            model::OutputNodeBase* outputNode = nullptr;
            switch (out.GetPortType())
            {
                case model::Port::PortType::boolean:
                    outputNode = GetModel().AddNode<model::OutputNode<bool>>(model::PortElements<bool>(out));
                    break;
                case model::Port::PortType::integer:
                    outputNode = GetModel().AddNode<model::OutputNode<int>>(model::PortElements<int>(out));
                    break;
                case model::Port::PortType::real:
                    outputNode = GetModel().AddNode<model::OutputNode<double>>(model::PortElements<double>(out));
                    break;
                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }

            ResetOutput(0, outputNode->GetOutputPort());
        }
    }

    void IRCompiledMap::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::DynamicMap::WriteToArchive(archiver);
    }

    void IRCompiledMap::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::DynamicMap::ReadFromArchive(archiver);
        Compile();
    }
}
}
