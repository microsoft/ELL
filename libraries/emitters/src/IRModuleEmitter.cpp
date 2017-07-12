////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModuleEmitter.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRModuleEmitter.h"
#include "EmitterException.h"
#include "IRAssemblyWriter.h"
#include "IRDiagnosticHandler.h"
#include "IRExecutionEngine.h"
#include "IRHeaderWriter.h"
#include "IRLoader.h"
#include "IRMetadata.h"
#include "IRSwigInterfaceWriter.h"

// utilities
#include "Files.h"

// llvm
#include "llvm/AsmParser/Parser.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_os_ostream.h"

// stl
#include <chrono>

namespace ell
{
namespace emitters
{
    namespace
    {
        static llvm::LLVMContext g_globalLLVMContext;
        static bool g_llvmIsInitialized = false;
        static std::unique_ptr<IRDiagnosticHandler> g_globalDiagnosticHandler = nullptr;
    }

    //
    // Public methods
    //

    //
    // Constructors
    //

    IRModuleEmitter::IRModuleEmitter(const std::string& moduleName)
        : _llvmContext(g_globalLLVMContext), _emitter(_llvmContext), _runtime(*this)
    {
        InitializeLLVM();
        InitializeGlobalPassRegistry();
        _pModule = _emitter.AddModule(moduleName);
        if (GetCompilerParameters().includeDiagnosticInfo)
        {
            DeclarePrintf();
        }
    }

    IRModuleEmitter::IRModuleEmitter(std::unique_ptr<llvm::Module> pModule)
        : _llvmContext(g_globalLLVMContext), _emitter(_llvmContext), _runtime(*this), _pModule(std::move(pModule))
    {
        InitializeLLVM();
        InitializeGlobalPassRegistry();
        if (GetCompilerParameters().includeDiagnosticInfo)
        {
            DeclarePrintf();
        }
    }

    //
    // Creating functions
    //

    void IRModuleEmitter::BeginMapPredictFunction(const std::string& functionName, NamedVariableTypeList& args)
    {
        BeginFunction(functionName, VariableType::Void, args);
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType)
    {
        auto currentPos = _emitter.GetCurrentInsertPoint();
        // assert(currentBlock != nullptr);
        IRFunctionEmitter newFunction = Function(functionName, returnType, ValueTypeList{}, true);
        _functionStack.emplace(newFunction, currentPos);
        return _functionStack.top().first;
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType, const ValueTypeList& args)
    {
        auto currentPos = _emitter.GetCurrentInsertPoint();
        // assert(currentBlock != nullptr);
        IRFunctionEmitter newFunction = Function(functionName, returnType, args, true);
        _functionStack.emplace(newFunction, currentPos);
        return _functionStack.top().first;
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType, const NamedVariableTypeList& args)
    {
        auto currentPos = _emitter.GetCurrentInsertPoint();
        // assert(currentBlock != nullptr);
        IRFunctionEmitter newFunction = Function(functionName, returnType, args, true);
        _functionStack.emplace(newFunction, currentPos);
        return _functionStack.top().first;
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, llvm::Type* returnType, const std::vector<llvm::Type*>& argTypes)
    {
        auto currentPos = _emitter.GetCurrentInsertPoint();
        // assert(currentBlock != nullptr);
        IRFunctionEmitter newFunction = Function(functionName, returnType, argTypes, true);
        _functionStack.emplace(newFunction, currentPos);
        return _functionStack.top().first;
    }

    IRFunctionEmitter IRModuleEmitter::BeginMainFunction()
    {
        return BeginFunction("main", VariableType::Void);
    }

    void IRModuleEmitter::EndFunction()
    {
        EndFunction(nullptr);
    }

    void IRModuleEmitter::EndFunction(llvm::Value* pReturn)
    {
        if (_functionStack.empty())
        {
            throw EmitterException(EmitterError::indexOutOfRange);
        }

        auto currentFunctionInfo = _functionStack.top();
        _functionStack.pop();

        auto currentFunction = currentFunctionInfo.first;
        auto previousPos = currentFunctionInfo.second;
        if (currentFunction.GetFunction() != nullptr)
        {
            // only put a return here if this block doesn't already have one
            auto pTerm = currentFunction.GetCurrentBlock()->getTerminator();
            if (pTerm == nullptr)
            {
                if (pReturn != nullptr)
                {
                    currentFunction.Return(pReturn);
                }
                else
                {
                    currentFunction.Return();
                }
            }
            currentFunction.ConcatRegions();
            currentFunction.CompleteFunction(GetCompilerParameters().optimize);
        }
        _emitter.SetCurrentInsertPoint(previousPos);
    }

    IRFunctionEmitter& IRModuleEmitter::GetCurrentFunction()
    {
        return _functionStack.top().first;
    }

    bool IRModuleEmitter::HasFunctionComments(const std::string& functionName)
    {
        return _functionComments.find(functionName) != _functionComments.end();
    }

    std::vector<std::string> IRModuleEmitter::GetFunctionComments(const std::string& functionName)
    {
        return _functionComments[functionName];
    }

    void IRModuleEmitter::SetFunctionComments(const std::string& functionName, const std::vector<std::string>& comments)
    {
        _functionComments[functionName] = comments;
    }

    std::vector<std::string> IRModuleEmitter::GetMetadata(const std::string& functionName, const std::string& tag)
    {
        if (!HasMetadata(functionName, tag))
        {
            throw EmitterException(EmitterError::metadataNotFound);
        }

        std::vector<std::string> values;
        if (functionName.length() == 0)
        {
            // Module metadata
            const llvm::NamedMDNode* metadata = _pModule->getNamedMetadata(tag);
            for (const auto& op : metadata->operands())
            {
                values.push_back(llvm::cast<llvm::MDString>(op->getOperand(0))->getString());
            }
        }
        else
        {
            // Function metadata
            auto pFunction = GetFunction(functionName);
            const llvm::MDNode* metadata = pFunction->getMetadata(tag);

            // llvm::Function::setMetadata appears to only support single entries?
            values.push_back(llvm::cast<llvm::MDString>(metadata->getOperand(0))->getString());
        }
        return values;
    }

    bool IRModuleEmitter::HasMetadata(const std::string& functionName, const std::string& tag)
    {
        if (functionName.length() == 0)
        {
            // Module metadata
            return (_pModule->getNamedMetadata(tag) != nullptr);
        }
        else
        {
            // Function metadata
            auto pFunction = GetFunction(functionName);
            if (pFunction == nullptr)
            {
                throw EmitterException(EmitterError::functionNotFound);
            }
            return (pFunction->getMetadata(tag) != nullptr);
        }
    }

    void IRModuleEmitter::InsertMetadata(const std::string& functionName, const std::string& tag, const std::string& value)
    {
        llvm::Metadata* metadataElements[] = { llvm::MDString::get(_emitter.GetContext(), value) };
        auto metadataNode = llvm::MDNode::get(_emitter.GetContext(), metadataElements);

        if (functionName.length() == 0)
        {
            // Module metadata
            auto metadata = _pModule->getOrInsertNamedMetadata(tag);
            metadata->addOperand(metadataNode);
        }
        else
        {
            // Function metadata
            auto pFunction = GetFunction(functionName);
            if (pFunction == nullptr)
            {
                throw EmitterException(EmitterError::functionNotFound);
            }

            pFunction->setMetadata(tag, metadataNode);
        }
    }

    //
    // Variable management
    //

    llvm::Value* IRModuleEmitter::EnsureEmitted(Variable& var)
    {
        llvm::Value* pVal = nullptr;
        if (var.HasEmittedName())
        {
            pVal = GetEmittedVariable(var.Scope(), var.EmittedName());
        }

        if (pVal == nullptr)
        {
            AllocateVariable(var);
            pVal = GetEmittedVariable(var.Scope(), var.EmittedName());
            if (pVal == nullptr)
            {
                pVal = EmitVariable(var);
            }
        }
        return pVal;
    }

    llvm::Value* IRModuleEmitter::LoadVariable(Variable& var)
    {
        llvm::Value* pVal = EnsureEmitted(var);
        if (!var.IsLiteral())
        {
            pVal = GetCurrentFunction().Load(pVal);
        }
        return pVal;
    }

    void IRModuleEmitter::SetVariable(Variable& var, llvm::Value* pDestination, int offset, llvm::Value* pValue)
    {
        auto currentFunction = GetCurrentFunction();
        assert(pValue != nullptr);
        if (var.IsScalar())
        {
            if (offset > 0)
            {
                throw EmitterException(EmitterError::indexOutOfRange);
            }
            currentFunction.Store(pDestination, pValue);
            return;
        }

        if (offset >= var.Dimension())
        {
            throw EmitterException(EmitterError::indexOutOfRange);
        }
        currentFunction.SetValueAt(pDestination, currentFunction.Literal(offset), pValue);
    }

    //
    // Variable and Constant creation
    //

    llvm::GlobalVariable* IRModuleEmitter::Constant(VariableType type, const std::string& name, double value)
    {
        return Global(name, _emitter.Type(type), _emitter.Literal(value), true);
    }

    llvm::GlobalVariable* IRModuleEmitter::ConstantArray(const std::string& name, const std::vector<double>& value)
    {
        return Global(name, _emitter.ArrayType(VariableType::Double, value.size()), _emitter.Literal(value), true);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(VariableType type, const std::string& name)
    {
        return Global(name, _emitter.Type(type), _emitter.Zero(type), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(llvm::Type* pType, const std::string& name)
    {
        return Global(name, pType, nullptr, false);
    }

    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(VariableType type, const std::string& name, const size_t size)
    {
        llvm::ArrayType* pArrayType = _emitter.ArrayType(type, size);
        return Global(name, pArrayType, InitializeArray(pArrayType), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(const std::string& name, llvm::Type* pType, const size_t size)
    {
        assert(pType != nullptr);

        llvm::ArrayType* pArrayType = llvm::ArrayType::get(pType, size);
        return Global(name, pArrayType, InitializeArray(pArrayType), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(const std::string& name, const std::vector<double>& value)
    {
        return Global(name, _emitter.ArrayType(VariableType::Double, value.size()), _emitter.Literal(value), false);
    }

    // This is the actual implementation --- we should call it something different and/or put it in IREmitter
    llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, llvm::Type* pType, llvm::Constant* pInitial, bool isConst)
    {
        return new llvm::GlobalVariable(*GetLLVMModule(), pType, isConst, llvm::GlobalValue::InternalLinkage, pInitial, name); // TODO: make sure we really want to return a new'd pointer
    }

    //
    // Functions
    //

    void IRModuleEmitter::DeclareFunction(const std::string& name, VariableType returnType)
    {
        _emitter.DeclareFunction(GetLLVMModule(), name, returnType, nullptr);
    }

    void IRModuleEmitter::DeclareFunction(const std::string& name, VariableType returnType, const ValueTypeList& arguments)
    {
        _emitter.DeclareFunction(GetLLVMModule(), name, returnType, &arguments);
    }

    void IRModuleEmitter::DeclareFunction(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments)
    {
        _emitter.DeclareFunction(GetLLVMModule(), name, returnType, arguments);
    }

    void IRModuleEmitter::DeclareFunction(const std::string& name, llvm::FunctionType* functionType)
    {
        _emitter.DeclareFunction(GetLLVMModule(), name, functionType);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, bool isPublic)
    {
        return Function(name, returnType, nullptr, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const ValueTypeList& arguments, bool isPublic)
    {
        return Function(name, returnType, &arguments, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const std::initializer_list<VariableType>& arguments, bool isPublic)
    {
        ValueTypeList valueTypeList = arguments;
        return Function(name, returnType, &valueTypeList, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments, bool isPublic)
    {
        llvm::Function* pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), arguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        // TODO: put the above IREmitter call in the IRFunctionEmitter constructor

        return IRFunctionEmitter(this, &_emitter, pFunction, arguments, name);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const ValueTypeList* pArguments, bool isPublic)
    {
        llvm::Function* pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), pArguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return IRFunctionEmitter(this, &_emitter, pFunction, name);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, llvm::Type* returnType, const std::vector<llvm::Type*>& argTypes, bool isPublic)
    {
        llvm::Function* pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), argTypes);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return IRFunctionEmitter(this, &_emitter, pFunction, name);
    }

    bool IRModuleEmitter::HasFunction(const std::string& name)
    {
        return GetLLVMModule()->getFunction(name) != nullptr;
    }

    llvm::Function* IRModuleEmitter::GetFunction(const std::string& name)
    {
        return GetLLVMModule()->getFunction(name);
    }

    llvm::Function* IRModuleEmitter::GetIntrinsic(llvm::Intrinsic::ID id, const std::initializer_list<VariableType>& arguments)
    {
        ValueTypeList valueTypeList = arguments;
        return _emitter.GetIntrinsic(GetLLVMModule(), id, valueTypeList);
    }

    //
    // Types
    //

    llvm::StructType* IRModuleEmitter::Struct(const std::string& name, const std::initializer_list<VariableType>& fields)
    {
        ValueTypeList valueTypeList = fields;
        return _emitter.Struct(name, valueTypeList);
    }

    llvm::StructType* IRModuleEmitter::Struct(const std::string& name, const std::vector<VariableType>& fields)
    {
        return _emitter.Struct(name, fields);
    }

    //
    // Code output / input
    //

    void IRModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format)
    {
        MachineCodeOutputOptions options;
        switch (format)
        {
            case ModuleOutputFormat::assembly:
            case ModuleOutputFormat::bitcode:
            case ModuleOutputFormat::ir:
            case ModuleOutputFormat::objectCode:
            {
                WriteToFile(filePath, format, options);
                break;
            }
            case ModuleOutputFormat::cHeader:
            {
                auto os = utilities::OpenOfstream(filePath);
                WriteToStream(os, format, options);
                break;
            }
            case ModuleOutputFormat::swigInterface:
            {
                // Write the swig interface file
                auto headerFilePath = filePath + ".h";
                auto os = utilities::OpenOfstream(filePath);
                WriteModuleSwigInterface(os, *this, utilities::GetFileName(headerFilePath));

                // Write the swig header file
                auto osHeader = utilities::OpenOfstream(headerFilePath);
                WriteModuleSwigHeader(osHeader, *this);
                break;
            }
            default:
                throw new EmitterException(EmitterError::notSupported);
        }
    }

    void IRModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format, const MachineCodeOutputOptions& options)
    {
        auto openFlags = (ModuleOutputFormat::bitcode == format || ModuleOutputFormat::objectCode == format) ? llvm::sys::fs::F_None : llvm::sys::fs::F_Text;
        std::error_code error;
        llvm::tool_output_file out(filePath, error, openFlags);
        if (error)
        {
            throw LLVMException(error);
        }

        WriteToLLVMStream(out.os(), format, options);

        if (out.os().has_error())
        {
            throw EmitterException(EmitterError::writeStreamFailed);
        }
        out.keep();
    }

    void IRModuleEmitter::WriteToStream(std::ostream& stream, ModuleOutputFormat format)
    {
        MachineCodeOutputOptions options;
        WriteToStream(stream, format, options);
    }

    void IRModuleEmitter::WriteToStream(std::ostream& os, ModuleOutputFormat format, const MachineCodeOutputOptions& options)
    {
        if (ModuleOutputFormat::cHeader == format)
        {
            WriteHeader(os);
        }
        else if (ModuleOutputFormat::swigInterface == format)
        {
            throw new EmitterException(EmitterError::notSupported, "swig only supports WriteToFile");
        }
        else
        {
            llvm::raw_os_ostream out(os);
            WriteToLLVMStream(out, format, options);
        }
    }

    void IRModuleEmitter::WriteToLLVMStream(llvm::raw_ostream& os, ModuleOutputFormat format, MachineCodeOutputOptions options)
    {
        const auto& params = GetCompilerParameters();

        // TODO: make this code nicer:
        if (options.targetDevice.triple == "")
        {
            options.targetDevice.triple = params.targetDevice.triple;
        }

        if (options.targetDevice.cpu == "")
        {
            options.targetDevice.triple = params.targetDevice.cpu;
        }

        if (options.targetDevice.features == "")
        {
            options.targetDevice.triple = params.targetDevice.features;
        }

        // optimization level
        if (ModuleOutputFormat::bitcode == format)
        {
            llvm::WriteBitcodeToFile(GetLLVMModule(), os);
        }
        else if (ModuleOutputFormat::ir == format)
        {
            GetLLVMModule()->print(os, nullptr);
        }
        else if (ModuleOutputFormat::assembly == format)
        {
            // TODO: fuse or replace options with compiler options
            GenerateMachineCode(os, *this, OutputFileType::CGFT_AssemblyFile, options);
        }
        else if (ModuleOutputFormat::objectCode == format)
        {
            // TODO: fuse or replace options with compiler options
            GenerateMachineCode(os, *this, OutputFileType::CGFT_ObjectFile, options);
        }
        else
        {
            throw new EmitterException(EmitterError::notSupported);
        }
    }

    void IRModuleEmitter::LoadIR(const std::string& text)
    {
        llvm::MemoryBufferRef buffer(text, "<string>"); // See Parser.cpp in LLVM code base for why...
        llvm::SMDiagnostic errorHandler;
        bool hadError = llvm::parseAssemblyInto(buffer, *GetLLVMModule(), errorHandler);
        if (hadError)
        {
            std::string message = errorHandler.getMessage(); //IRLoader::ErrorToString(errorHandler);
            throw EmitterException(EmitterError::parserError, message);
        }
    }

    void IRModuleEmitter::WriteHeader(std::ostream& os)
    {
        WriteModuleHeader(os, *this);
    }

    //
    // Optimization
    //

    void IRModuleEmitter::Optimize()
    {
        IRModuleOptimizer optimizer;
        optimizer.AddStandardPasses();
        Optimize(optimizer);
    }

    void IRModuleEmitter::Optimize(IRModuleOptimizer& optimizer)
    {
        optimizer.Run(GetLLVMModule());
    }

    //
    // Code execution
    //

    void IRModuleEmitter::SetTargetMachine(llvm::TargetMachine* pMachine)
    {
        assert(pMachine != nullptr);
        GetLLVMModule()->setDataLayout(pMachine->createDataLayout());
    }

    //
    // Helpers, standard C Runtime functions, and debug support
    //

    void IRModuleEmitter::DeclarePrintf()
    {
        llvm::FunctionType* type = llvm::TypeBuilder<int(char*, ...), false>::get(_emitter.GetContext());
        DeclareFunction("printf", type);
    }

    void IRModuleEmitter::DeclareMalloc()
    {
        DeclareFunction("malloc", VariableType::BytePointer, { VariableType::Int64 });
    }

    void IRModuleEmitter::DeclareFree()
    {
        DeclareFunction("free", VariableType::Void, { VariableType::BytePointer });
    }

    template <>
    void IRModuleEmitter::DeclareGetClockMilliseconds<std::chrono::steady_clock>()
    {
        DeclareFunction("ELL_GetSteadyClockMilliseconds", VariableType::Double);
    }

    template <>
    void IRModuleEmitter::DeclareGetClockMilliseconds<std::chrono::system_clock>()
    {
        DeclareFunction("ELL_GetSystemClockMilliseconds", VariableType::Double);
    }

    IRFunctionEmitter IRModuleEmitter::BeginMainDebugFunction()
    {
        DeclarePrintf();
        return BeginMainFunction();
    }

    IRDiagnosticHandler& IRModuleEmitter::GetDiagnosticHandler()
    {
        return *g_globalDiagnosticHandler;
    }

    void IRModuleEmitter::DebugDump()
    {
        GetLLVMModule()->dump();
    }

    //
    // low-level LLVM-related functionality
    //

    std::unique_ptr<llvm::Module> IRModuleEmitter::TransferOwnership()
    {
        auto result = std::move(_pModule);
        _pModule = nullptr;
        return result;
    }

    void IRModuleEmitter::SetTargetTriple(const std::string& triple)
    {
        GetLLVMModule()->setTargetTriple(triple);
    }

    void IRModuleEmitter::SetTargetDataLayout(const std::string& dataLayout)
    {
        GetLLVMModule()->setDataLayout(llvm::DataLayout(dataLayout));
    }

    void IRModuleEmitter::AddPreprocessorDefinition(const std::string& name, const std::string& value)
    {
        _preprocessorDefinitions.emplace_back(name, value);
    }

    //
    // Metadata
    //

    void IRModuleEmitter::IncludeInHeader(const std::string& functionName)
    {
        InsertMetadata(functionName, c_declareInHeaderTagName);
    }

    void IRModuleEmitter::IncludeTypeInHeader(const std::string& typeName)
    {
        InsertMetadata("", c_declareInHeaderTagName, typeName);
    }

    void IRModuleEmitter::IncludeInCallbackInterface(const std::string& functionName, const std::string& nodeName)
    {
        InsertMetadata(functionName, c_callbackFunctionTagName, nodeName);
    }

    //
    // Protected methods
    //

    //
    // Private methods
    //

    //
    // Internal variable and function creation implementation
    //

    llvm::Value* IRModuleEmitter::GetEmittedVariable(const VariableScope scope, const std::string& name)
    {
        switch (scope)
        {
            case VariableScope::literal:
                return _literals.Get(name);

            case VariableScope::global:
                return _globals.Get(name);

            case VariableScope::local:
            case VariableScope::input:
            case VariableScope::output:
                return GetCurrentFunction().GetEmittedVariable(scope, name);

            default:
                throw EmitterException(EmitterError::variableScopeNotSupported);
        }
    }

    llvm::Value* IRModuleEmitter::EmitVariable(Variable& var)
    {
        assert(var.HasEmittedName());
        switch (var.Type())
        {
            case VariableType::Byte:
                return EmitVariable<uint8_t>(var);
            case VariableType::Int32:
                return EmitVariable<int>(var);
            case VariableType::Int64:
                return EmitVariable<int64_t>(var);
            case VariableType::Float:
                return EmitVariable<float>(var);
            case VariableType::Double:
                return EmitVariable<double>(var);
            default:
                break;
        }
        throw EmitterException(EmitterError::variableTypeNotSupported);
    }

    llvm::Function::LinkageTypes IRModuleEmitter::Linkage(bool isPublic)
    {
        return isPublic ? llvm::Function::LinkageTypes::ExternalLinkage : llvm::Function::LinkageTypes::InternalLinkage;
    }

    llvm::ConstantAggregateZero* IRModuleEmitter::InitializeArray(llvm::ArrayType* pType)
    {
        assert(pType != nullptr);
        return llvm::ConstantAggregateZero::get(pType);
    }

    //
    // Global LLVM state management
    //
    void IRModuleEmitter::InitializeLLVM()
    {
        if (g_llvmIsInitialized)
        {
            return;
        }

        // All targets
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmPrinters();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllDisassemblers();

        // Native target (perhaps unnecessary, since we're initializing _all_ the targets above)
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        llvm::InitializeNativeTargetDisassembler();

        // Create a diagnostic handler to record if there was an error
        g_globalDiagnosticHandler = std::unique_ptr<IRDiagnosticHandler>(new IRDiagnosticHandler(g_globalLLVMContext));
    }

    llvm::PassRegistry* IRModuleEmitter::InitializeGlobalPassRegistry()
    {
        // Get the global pass registry
        llvm::PassRegistry* registry = llvm::PassRegistry::getPassRegistry();

        // Initialize all of the optimization passes (probably unnecessary)
        llvm::initializeCore(*registry);
        llvm::initializeTransformUtils(*registry);
        llvm::initializeScalarOpts(*registry);
        // llvm::initializeObjCARCOpts(*registry);
        llvm::initializeVectorization(*registry);
        llvm::initializeInstCombine(*registry);
        llvm::initializeIPO(*registry); // IPO == interprocedural optimizations
        llvm::initializeInstrumentation(*registry);
        llvm::initializeAnalysis(*registry);
        llvm::initializeCodeGen(*registry);
        llvm::initializeGlobalISel(*registry);
        llvm::initializeTarget(*registry);

        return registry;
    }
}
}
