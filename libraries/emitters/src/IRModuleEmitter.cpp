////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModuleEmitter.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRModuleEmitter.h"
#include "EmitterException.h"
#include "IRAssemblyEmitter.h"
#include "IRExecutionEngine.h"
#include "IRLoader.h"

// utilities
#include "Files.h"

namespace ell
{
namespace emitters
{
    namespace
    {
        static llvm::LLVMContext g_globalLLVMContext;
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
    }

    IRModuleEmitter::IRModuleEmitter(std::unique_ptr<llvm::Module> pModule)
        : _llvmContext(g_globalLLVMContext), _emitter(_llvmContext), _runtime(*this), _pModule(std::move(pModule))
    {
        InitializeLLVM();
        InitializeGlobalPassRegistry();
    }

    //
    // Creating functions
    //

    void IRModuleEmitter::BeginTopLevelFunction(const std::string& functionName, NamedVariableTypeList& args)
    {
        // TODO
        // ugh:
        if (GetCompilerParameters().includeDiagnosticInfo)
        {
            DeclarePrintf();
        }
        BeginFunction(functionName, VariableType::Void, args);
    }

    void IRModuleEmitter::EndTopLevelFunction()
    {
        EndFunction();
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType, NamedVariableTypeList& args)
    {
        auto currentBlock = _emitter.GetCurrentBlock();
        // assert(currentBlock != nullptr);
        IRFunctionEmitter currentFunction = Function(functionName, returnType, args, true);
        _functionStack.emplace(currentFunction, currentBlock);
        return _functionStack.top().first;
    }

    void IRModuleEmitter::EndFunction()
    {
        if (_functionStack.empty())
        {
            throw EmitterException(EmitterError::indexOutOfRange);
        }

        // TODO: assert passed-in function is same as top of function stack
        auto currentFunctionInfo = _functionStack.top();
        _functionStack.pop();

        auto currentFunction = currentFunctionInfo.first;
        auto previousBlock = currentFunctionInfo.second;
        if (currentFunction.GetFunction() != nullptr)
        {
            currentFunction.Return();
            currentFunction.ConcatRegions();
            currentFunction.Complete(GetCompilerParameters().optimize);
        }
        _emitter.SetCurrentBlock(previousBlock);
    }
    
    IRFunctionEmitter& IRModuleEmitter::GetCurrentFunction()
    {
        return _functionStack.top().first;
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

    llvm::GlobalVariable* IRModuleEmitter::Constant(const std::string& name, const std::vector<double>& value)
    {
        return Global(name, _emitter.ArrayType(VariableType::Double, value.size()), _emitter.Literal(value), true);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(VariableType type, const std::string& name)
    {
        return Global(name, _emitter.Type(type), _emitter.Zero(type), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(VariableType type, const std::string& name, const size_t size)
    {
        llvm::ArrayType* pArrayType = _emitter.ArrayType(type, size);
        return Global(name, pArrayType, InitializeArray(pArrayType), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(llvm::Type* pType, const std::string& name)
    {
        return Global(name, pType, nullptr, false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, llvm::Type* pType, const size_t size)
    {
        assert(pType != nullptr);

        llvm::ArrayType* pArrayType = llvm::ArrayType::get(pType, size);
        return Global(name, pArrayType, InitializeArray(pArrayType), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, const std::vector<double>& value)
    {
        return Global(name, _emitter.ArrayType(VariableType::Double, value.size()), _emitter.Literal(value), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, llvm::Type* pType, llvm::Constant* pInitial, bool isConst)
    {
        return new llvm::GlobalVariable(*GetLLVMModule(), pType, isConst, llvm::GlobalValue::InternalLinkage, pInitial, name);
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

    void IRModuleEmitter::DeclareFunction(const std::string& name, llvm::FunctionType* returnType)
    {
        _emitter.DeclareFunction(GetLLVMModule(), name, returnType);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, bool isPublic)
    {
        return Function(name, returnType, nullptr, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const ValueTypeList& arguments, bool isPublic)
    {
        return Function(name, returnType, &arguments, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const std::initializer_list<VariableType> arguments, bool isPublic)
    {
        _valueTypeList = arguments;
        return Function(name, returnType, &_valueTypeList, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments, bool isPublic)
    {
        llvm::Function* pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), arguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        // TODO: put the above IREmitter call in the IRFunctionEmitter constructor

        return IRFunctionEmitter(this, &_emitter, pFunction, arguments);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const ValueTypeList* pArguments, bool isPublic)
    {
        llvm::Function* pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), pArguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return IRFunctionEmitter(this, &_emitter, pFunction);
    }

    bool IRModuleEmitter::HasFunction(const std::string& name)
    {
        return GetLLVMModule()->getFunction(name) != nullptr;
    }

    llvm::Function* IRModuleEmitter::GetFunction(const std::string& name)
    {
        return GetLLVMModule()->getFunction(name);
    }

    llvm::Function* IRModuleEmitter::GetIntrinsic(llvm::Intrinsic::ID id, std::initializer_list<VariableType> arguments)
    {
        _valueTypeList = arguments;
        return _emitter.GetIntrinsic(GetLLVMModule(), id, _valueTypeList);
    }

    //
    // Types
    //

    llvm::StructType* IRModuleEmitter::Struct(const std::string& name, std::initializer_list<VariableType> fields)
    {
        _valueTypeList = fields;
        return _emitter.Struct(name, _valueTypeList);
    }

    //
    // Code output / input
    //

    void IRModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format)
    {
        MachineCodeOutputOptions options;
        WriteToFile(filePath, format, options);
    }

    void IRModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format, const MachineCodeOutputOptions& options)
    {
        auto openFlags = (ModuleOutputFormat::ir == format) ? llvm::sys::fs::F_None : llvm::sys::fs::F_Text;
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

    void
    IRModuleEmitter::WriteToStream(std::ostream& stream, ModuleOutputFormat format)
    {
        MachineCodeOutputOptions options;
        WriteToStream(stream, format, options);
    }

    void IRModuleEmitter::WriteToStream(std::ostream& os, ModuleOutputFormat format, const MachineCodeOutputOptions& options)
    {
        llvm::raw_os_ostream out(os);
        WriteToLLVMStream(out, format, options);
    }

    void IRModuleEmitter::WriteToLLVMStream(llvm::raw_ostream& os, ModuleOutputFormat format, const MachineCodeOutputOptions& options)
    {
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
            GenerateMachineCode(os, *GetLLVMModule(), OutputFileType::CGFT_AssemblyFile, options);
        }
        else
        {
            throw new EmitterException(EmitterError::notSupported);
        }
    }

    void IRModuleEmitter::LoadAssembly(const std::string& text)
    {
        llvm::MemoryBufferRef buffer(text, "<string>"); // See Parser.cpp in LLVM code base for why...
        llvm::SMDiagnostic err;
        if (!llvm::parseAssemblyInto(buffer, *GetLLVMModule(), err))
        {
            throw EmitterException(EmitterError::parserError, IRLoader::ErrorToString(err));
        }
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

    IRFunctionEmitter IRModuleEmitter::AddMain()
    {
        return Function("main", VariableType::Void, true);
    }

    void IRModuleEmitter::DeclarePrintf()
    {
        llvm::FunctionType* type = llvm::TypeBuilder<void(char*, ...), false>::get(_emitter.GetContext());
        DeclareFunction("printf", type);
    }

    void IRModuleEmitter::DeclareMalloc()
    {
        _valueTypeList = { VariableType::Int64 };
        DeclareFunction("malloc", VariableType::BytePointer, _valueTypeList);
    }

    void IRModuleEmitter::DeclareFree()
    {
        _valueTypeList = { VariableType::BytePointer };
        DeclareFunction("free", VariableType::Void, _valueTypeList);
    }

    IRFunctionEmitter IRModuleEmitter::AddMainDebug()
    {
        DeclarePrintf();
        return AddMain();
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