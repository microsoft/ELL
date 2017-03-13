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

// llvm
#include "llvm/Transforms/Utils/Cloning.h"

// stl
#include <sstream>

namespace ell
{
namespace model
{
    IRCompiledMap::IRCompiledMap(IRCompiledMap&& other)
        : CompiledMap(std::move(other), other._functionName), _moduleName(std::move(other._moduleName)), _module(std::move(other._module)), _executionEngine(std::move(other._executionEngine))
    {
        if (_executionEngine)
        {
            SetComputeFunction();
        }
    }

    // private constructor:
    IRCompiledMap::IRCompiledMap(DynamicMap other, const std::string& functionName, std::unique_ptr<emitters::IRModuleEmitter> module)
        : CompiledMap(std::move(other), functionName), _module(std::move(module))
    {
    }

    bool IRCompiledMap::IsValid() const
    {
        return _module != nullptr && _module->IsValid();
    }

    emitters::IRExecutionEngine& IRCompiledMap::GetJitter()
    {
        EnsureExecutionEngine();
        return *_executionEngine;
    }

    void IRCompiledMap::EnsureExecutionEngine() const
    {
        if (!_executionEngine)
        {
            auto moduleClone = std::unique_ptr<llvm::Module>(llvm::CloneModule(_module->GetLLVMModule()));
            _executionEngine = std::make_unique<emitters::IRExecutionEngine>(std::move(moduleClone));
            SetComputeFunction();
        }
    }

    void IRCompiledMap::SetComputeFunction() const
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
        EnsureExecutionEngine();
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
        EnsureExecutionEngine();
        if (GetInput(0)->GetOutputPort().GetType() != node->GetOutputPort().GetType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        std::get<ComputeFunction<int>>(_computeInputFunction)(inputValues.data());
    }

    void IRCompiledMap::SetNodeInput(model::InputNode<double>* node, const std::vector<double>& inputValues) const
    {
        EnsureExecutionEngine();
        if (GetInput(0)->GetOutputPort().GetType() != node->GetOutputPort().GetType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        std::get<ComputeFunction<double>>(_computeInputFunction)(inputValues.data());
    }

    std::vector<bool> IRCompiledMap::ComputeBoolOutput(const model::PortElementsBase& outputs) const
    {
        EnsureExecutionEngine();
        if (GetOutput(0).GetPortType() != model::Port::PortType::boolean)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        // Terrible hack to create a std::vector<bool>
        return std::vector<bool>((bool*)(std::get<utilities::ConformingVector<bool>>(_cachedOutput).data()), (bool*)(std::get<utilities::ConformingVector<bool>>(_cachedOutput).data() + std::get<utilities::ConformingVector<bool>>(_cachedOutput).size()));
    }

    std::vector<int> IRCompiledMap::ComputeIntOutput(const model::PortElementsBase& outputs) const
    {
        EnsureExecutionEngine();
        if (GetOutput(0).GetPortType() != model::Port::PortType::integer)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        return std::get<utilities::ConformingVector<int>>(_cachedOutput);
    }

    std::vector<double> IRCompiledMap::ComputeDoubleOutput(const model::PortElementsBase& outputs) const
    {
        EnsureExecutionEngine();
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
}
}
