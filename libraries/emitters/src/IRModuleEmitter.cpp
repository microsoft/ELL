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
#include "IRHeaderWriter.h"
#include "IRLoader.h"
#include "IRMetadata.h"
#include "IRSwigInterfaceWriter.h"
#include "LLVMUtilities.h"
#include "TargetDevice.h"

#include <utilities/include/Files.h>
#include <utilities/include/Logger.h>

#include <llvm/ADT/Triple.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>

namespace ell
{
namespace emitters
{
    using namespace utilities::logging;
    using utilities::logging::Log;

    //
    // Constructors
    //

    IRModuleEmitter::IRModuleEmitter(const std::string& moduleName, const CompilerOptions& parameters) :
        _llvmContext(std::make_unique<llvm::LLVMContext>()),
        _llvmModule(std::make_unique<llvm::Module>(moduleName, *_llvmContext)),
        _emitter(*this, *_llvmContext),
        _runtime(*this),
        _threadPool(*this),
        _profiler(*this, parameters.profile)
    {
        InitializeLLVM();
        InitializeGlobalPassRegistry();

        SetCompilerOptions(parameters);
        if (GetCompilerOptions().includeDiagnosticInfo)
        {
            DeclarePrintf();
        }

        _profiler.Init();
    }

    void IRModuleEmitter::SetCompilerOptions(const CompilerOptions& parameters)
    {
        auto params = parameters;
        CompleteCompilerOptions(params);

        // Call base class implementation
        ModuleEmitter::SetCompilerOptions(params);

        // Set IR-specific parameters
        const auto& targetDevice = GetCompilerOptions().targetDevice;
        if (targetDevice.triple.empty())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Target device triple is empty or could not be determined");
        }
        SetTargetTriple(targetDevice.triple);
        llvm::DataLayout dataLayout(GetCompilerOptions().targetDevice.dataLayout);
        GetLLVMModule()->setDataLayout(dataLayout);
    }

    void IRModuleEmitter::CompleteCompilerOptions(CompilerOptions& parameters)
    {
        CompleteTargetDevice(parameters.targetDevice);
    }

    //
    // Creating functions
    //

    void IRModuleEmitter::BeginMapPredictFunction(const std::string& functionName, NamedVariableTypeList& args)
    {
        IRFunctionEmitter& function = BeginFunction(functionName, VariableType::Void, args);
        function.SetAttributeForArguments(IRFunctionEmitter::Attributes::NoAlias);

        // store context variable so the callbacks can find it later.
        auto context = function.GetFunctionArgument("context");
        auto globalContext = GlobalPointer(GetModuleName() + "_context", emitters::VariableType::Byte);
        function.Store(globalContext, context);
    }

    void IRModuleEmitter::EndMapPredictFunction()
    {
        EndFunction();

        // now generate the public reset function that combines all the node level reset functions.
        IRFunctionEmitter& resetFunction = BeginFunction(GetModuleName() + "_Reset", VariableType::Void);
        resetFunction.IncludeInHeader();
        for (auto name : _resetFunctions)
        {
            resetFunction.Call(name);
        }
        EndFunction();
    }

    /// <summary> Begin a new function for resetting a given node. Each node that needs to implement
    /// reset calls this and implements their own reset logic.  The IRModuleEmitter wraps all that
    /// in a master model_Reset function which is exposed in the API. </summary>
    IRFunctionEmitter& IRModuleEmitter::BeginResetFunction(const std::string& functionName)
    {
        IRFunctionEmitter& function = BeginFunction(functionName, VariableType::Void);
        _resetFunctions.push_back(functionName);
        return function;
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType)
    {
        _functions[functionName] = FunctionDeclaration(functionName, returnType);
        return BeginFunction(functionName, _emitter.Type(returnType));
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, LLVMType returnType)
    {
        return BeginFunction(functionName, returnType, NamedVariableTypeList{});
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType, const VariableTypeList& args)
    {
        NamedVariableTypeList fake;
        for (auto t : args)
        {
            fake.push_back({ "", t });
        }
        _functions[functionName] = FunctionDeclaration(functionName, returnType, fake);
        return BeginFunction(functionName, _emitter.Type(returnType), _emitter.GetLLVMTypes(args));
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, VariableType returnType, const NamedVariableTypeList& args)
    {
        _functions[functionName] = FunctionDeclaration(functionName, returnType, args);
        return BeginFunction(functionName, _emitter.Type(returnType), args);
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, LLVMType returnType, const NamedVariableTypeList& args)
    {
        if (_functions.find(functionName) == _functions.end())
        {
            _functions[functionName] = FunctionDeclaration(functionName, VariableType::Custom, args);
        }
        Log() << "Begin emitting IR for function " << functionName << EOL;
        auto currentPos = _emitter.GetCurrentInsertPoint();
        IRFunctionEmitter newFunction = Function(functionName, returnType, args, false);
        _functionStack.emplace(newFunction, currentPos);
        return _functionStack.top().first;
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, LLVMType returnType, const std::vector<LLVMType>& argTypes)
    {
        Log() << "Begin emitting IR for function " << functionName << EOL;
        auto currentPos = _emitter.GetCurrentInsertPoint();
        IRFunctionEmitter newFunction = Function(functionName, returnType, argTypes, false);
        _functionStack.emplace(newFunction, currentPos);
        return _functionStack.top().first;
    }

    IRFunctionEmitter& IRModuleEmitter::BeginFunction(const std::string& functionName, LLVMType returnType, const NamedLLVMTypeList& args)
    {
        Log() << "Begin emitting IR for function " << functionName << EOL;
        auto currentPos = _emitter.GetCurrentInsertPoint();
        IRFunctionEmitter newFunction = Function(functionName, returnType, args, false);
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

    void IRModuleEmitter::EndFunction(LLVMValue pReturn)
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
                Log() << "Function " << currentFunction.GetFunctionName() << " does not have a terminator. " << EOL;
                if (pReturn != nullptr)
                {
                    Log() << "Returning non-void value." << EOL;
                    currentFunction.Return(pReturn);
                }
                else
                {
                    Log() << "Returning void." << EOL;
                    currentFunction.Return();
                }
            }
            else
            {
                Log() << "Function " << currentFunction.GetFunctionName() << " already has a terminator" << EOL;
            }
            currentFunction.ConcatRegions();
            currentFunction.CompleteFunction();
        }
        _emitter.SetCurrentInsertPoint(previousPos);

        Log() << "End emitting of function " << currentFunction.GetFunctionName() << EOL;
    }

    IRFunctionEmitter& IRModuleEmitter::GetCurrentFunction()
    {
        if (_functionStack.empty())
        {
            throw EmitterException(EmitterError::indexOutOfRange, "There is no current function on the stack");
        }
        return _functionStack.top().first;
    }

    FunctionDeclaration& IRModuleEmitter::GetFunctionDeclaration(const std::string& name)
    {
        return _functions[name];
    }

    // Module metadata
    std::vector<std::vector<std::string>> IRModuleEmitter::GetMetadata(const std::string& tag)
    {
        if (!HasMetadata(tag))
        {
            throw EmitterException(EmitterError::metadataNotFound);
        }

        std::vector<std::vector<std::string>> values;
        const llvm::NamedMDNode* metadata = _llvmModule->getNamedMetadata(tag);
        for (const auto& op : metadata->operands()) // op is an MDNode*
        {
            std::vector<std::string> opValues;
            for (const auto& innerOp : op->operands())
            {
                opValues.push_back(llvm::cast<llvm::MDString>(innerOp)->getString());
            }
            values.push_back(opValues);
        }
        return values;
    }

    // Function metadata
    std::vector<std::string> IRModuleEmitter::GetFunctionMetadata(const std::string& functionName, const std::string& tag)
    {
        if (!HasFunctionMetadata(functionName, tag))
        {
            throw EmitterException(EmitterError::metadataNotFound);
        }

        std::vector<std::string> values;
        auto pFunction = GetFunction(functionName);
        const llvm::MDNode* metadata = pFunction->getMetadata(tag);
        for (const auto& op : metadata->operands())
        {
            // llvm::Function::setMetadata appears to only support single entries?
            values.push_back(llvm::cast<llvm::MDString>(op)->getString());
        }
        return values;
    }

    // Module metadata
    bool IRModuleEmitter::HasMetadata(const std::string& tag)
    {
        return (_llvmModule->getNamedMetadata(tag) != nullptr);
    }

    // Function metadata
    bool IRModuleEmitter::HasFunctionMetadata(const std::string& functionName, const std::string& tag)
    {
        auto pFunction = GetFunction(functionName);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return (pFunction->getMetadata(tag) != nullptr);
    }

    // Module metadata
    void IRModuleEmitter::InsertMetadata(const std::string& tag, const std::vector<std::string>& values)
    {
        std::vector<llvm::Metadata*> metadataElements;
        for (const auto& value : values)
        {
            metadataElements.push_back({ llvm::MDString::get(_emitter.GetContext(), value) });
        }

        auto metadataNode = llvm::MDNode::get(_emitter.GetContext(), metadataElements);
        auto metadata = _llvmModule->getOrInsertNamedMetadata(tag);
        metadata->addOperand(metadataNode);
    }

    // Function metadata
    void IRModuleEmitter::InsertFunctionMetadata(const std::string& functionName, const std::string& tag, const std::vector<std::string>& values)
    {
        auto pFunction = GetFunction(functionName);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }

        InsertFunctionMetadata(pFunction, tag, values);
    }

    void IRModuleEmitter::InsertFunctionMetadata(LLVMFunction function, const std::string& tag, const std::vector<std::string>& values)
    {
        assert(function);

        std::vector<llvm::Metadata*> metadataElements;
        for (const auto& value : values)
        {
            metadataElements.push_back({ llvm::MDString::get(_emitter.GetContext(), value) });
        }

        auto metadataNode = llvm::MDNode::get(_emitter.GetContext(), metadataElements);
        function->setMetadata(tag, metadataNode);
    }

    //
    // Variable management
    //

    LLVMValue IRModuleEmitter::EnsureEmitted(Variable& var)
    {
        LLVMValue pVal = nullptr;
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

    LLVMValue IRModuleEmitter::LoadVariable(Variable& var)
    {
        LLVMValue pVal = EnsureEmitted(var);
        if (!var.IsLiteral())
        {
            pVal = GetCurrentFunction().Load(pVal);
        }
        return pVal;
    }

    //
    // Variable and Constant creation
    //

    llvm::GlobalVariable* IRModuleEmitter::Constant(VariableType type, const std::string& name, double value)
    {
        return AddGlobal(name, _emitter.Type(type), _emitter.Literal(value), true);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(VariableType type, const std::string& name)
    {
        return AddGlobal(name, _emitter.Type(type), _emitter.Zero(type), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::Global(LLVMType pType, const std::string& name)
    {
        auto initializer = ZeroInitializer(pType);
        return AddGlobal(name, pType, initializer, false);
    }

    llvm::GlobalVariable* IRModuleEmitter::GlobalPointer(const std::string& name, VariableType type)
    {
        llvm::PointerType* pointerType = _emitter.Type(type)->getPointerTo();
        return AddGlobal(name, pointerType, _emitter.NullPointer(pointerType), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(VariableType type, const std::string& name, const size_t size)
    {
        llvm::ArrayType* pArrayType = _emitter.ArrayType(type, size);
        return AddGlobal(name, pArrayType, ZeroInitializer(pArrayType), false);
    }

    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(const std::string& name, LLVMType pType, const size_t size)
    {
        assert(pType != nullptr);

        llvm::ArrayType* pArrayType = llvm::ArrayType::get(pType, size);
        return AddGlobal(name, pArrayType, ZeroInitializer(pArrayType), false);
    }

    // This function has the actual implementation for all the above Global/GlobalArray() methods
    llvm::GlobalVariable* IRModuleEmitter::AddGlobal(const std::string& name, LLVMType pType, llvm::Constant* pInitial, bool isConst)
    {
        _llvmModule->getOrInsertGlobal(name, pType);
        auto global = _llvmModule->getNamedGlobal(name);
        global->setInitializer(pInitial);
        global->setConstant(isConst);
        global->setExternallyInitialized(false);
        global->setLinkage(llvm::GlobalValue::LinkageTypes::InternalLinkage);
        assert(llvm::isa<llvm::GlobalVariable>(global));
        return llvm::cast<llvm::GlobalVariable>(global);
    }

    //
    // Functions
    //

    LLVMFunction IRModuleEmitter::DeclareFunction(const std::string& name, VariableType returnType)
    {
        return _emitter.DeclareFunction(GetLLVMModule(), name, returnType);
    }

    LLVMFunction IRModuleEmitter::DeclareFunction(const std::string& name, VariableType returnType, const VariableTypeList& arguments)
    {
        return _emitter.DeclareFunction(GetLLVMModule(), name, returnType, arguments);
    }

    LLVMFunction IRModuleEmitter::DeclareFunction(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments)
    {
        return _emitter.DeclareFunction(GetLLVMModule(), name, returnType, arguments);
    }

    LLVMFunction IRModuleEmitter::DeclareFunction(const std::string& name, llvm::FunctionType* functionType)
    {
        return _emitter.DeclareFunction(GetLLVMModule(), name, functionType);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, bool isPublic)
    {
        return Function(name, returnType, nullptr, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const VariableTypeList& arguments, bool isPublic)
    {
        return Function(name, returnType, &arguments, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const std::initializer_list<VariableType>& arguments, bool isPublic)
    {
        VariableTypeList valueTypeList = arguments;
        return Function(name, returnType, &valueTypeList, isPublic);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments, bool isPublic)
    {
        LLVMFunction pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), arguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        // TODO: put the above IREmitter call in the IRFunctionEmitter constructor

        return IRFunctionEmitter(this, pFunction, arguments, name);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, LLVMType returnType, const NamedVariableTypeList& arguments, bool isPublic)
    {
        LLVMFunction pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), arguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        // TODO: put the above IREmitter call in the IRFunctionEmitter constructor

        return IRFunctionEmitter(this, pFunction, arguments, name);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, VariableType returnType, const VariableTypeList* pArguments, bool isPublic)
    {
        LLVMFunction pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), pArguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return IRFunctionEmitter(this, pFunction, name);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, LLVMType returnType, const std::vector<LLVMType>& argTypes, bool isPublic)
    {
        LLVMFunction pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), argTypes);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return IRFunctionEmitter(this, pFunction, name);
    }

    IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, LLVMType returnType, const NamedLLVMTypeList& arguments, bool isPublic)
    {
        LLVMFunction pFunction = _emitter.Function(GetLLVMModule(), name, returnType, Linkage(isPublic), arguments);
        if (pFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return IRFunctionEmitter(this, pFunction, name);
    }

    bool IRModuleEmitter::HasFunction(const std::string& name)
    {
        return GetLLVMModule()->getFunction(name) != nullptr;
    }

    LLVMFunction IRModuleEmitter::GetFunction(const std::string& name)
    {
        return GetLLVMModule()->getFunction(name);
    }

    LLVMFunction IRModuleEmitter::GetIntrinsic(llvm::Intrinsic::ID id, const std::initializer_list<VariableType>& arguments)
    {
        VariableTypeList valueTypeList = arguments;
        return _emitter.GetIntrinsic(GetLLVMModule(), id, valueTypeList);
    }

    LLVMFunction IRModuleEmitter::GetIntrinsic(llvm::Intrinsic::ID id, const std::initializer_list<LLVMType>& arguments)
    {
        LLVMTypeList valueTypeList = arguments;
        return _emitter.GetIntrinsic(GetLLVMModule(), id, valueTypeList);
    }

    //
    // Types
    //

    llvm::StructType* IRModuleEmitter::GetOrCreateStruct(const std::string& name, const NamedVariableTypeList& fields)
    {
        NamedLLVMTypeList llvmFields;
        for (auto& field : fields)
        {
            llvmFields.emplace_back(field.first, _emitter.Type(field.second));
        }
        return GetOrCreateStruct(name, llvmFields);
    }

    llvm::StructType* IRModuleEmitter::GetOrCreateStruct(const std::string& name, const NamedLLVMTypeList& fields)
    {
        std::vector<std::string> fieldNames;
        std::vector<LLVMType> types;
        for (const auto& field : fields)
        {
            fieldNames.push_back(field.first);
            types.push_back(field.second);
        }

        auto structType = GetOrCreateStruct(name, types);
        auto tag = GetStructFieldsTagName(structType);
        InsertMetadata(tag, fieldNames);
        return structType;
    }

    llvm::StructType* IRModuleEmitter::GetOrCreateStruct(const std::string& name, const LLVMTypeList& fields)
    {
        if (auto structType = _emitter.GetStruct(name))
        {
            // Check that existing struct fields match the ones we're trying to create
            auto structFields = structType->elements();
            if (structFields.size() != fields.size())
            {
                throw EmitterException(EmitterError::badStructDefinition, "Fields don't match existing struct of same name");
            }
            else
            {
                for (size_t fieldIndex = 0; fieldIndex < fields.size(); ++fieldIndex)
                {
                    if (fields[fieldIndex] != structFields[fieldIndex])
                    {
                        throw EmitterException(EmitterError::badStructDefinition, "Fields don't match existing struct of same name");
                    }
                }
            }
            return structType;
        }

        return _emitter.DeclareStruct(name, fields);
    }

    llvm::StructType* IRModuleEmitter::GetAnonymousStructType(const LLVMTypeList& fieldTypes, bool packed)
    {
        return _emitter.GetAnonymousStructType(fieldTypes, packed);
    }

    llvm::StructType* IRModuleEmitter::GetStruct(const std::string& name)
    {
        return _emitter.GetStruct(name);
    }

    //
    // Code output / input
    //

    void IRModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format)
    {
        MachineCodeOutputOptions options;
        auto compilerOptions = GetCompilerOptions();
        options.targetDevice = compilerOptions.targetDevice;
        if (compilerOptions.optimize)
        {
            options.optimizationLevel = OptimizationLevel::Aggressive;
        }

        options.verifyModule = true;
        options.floatFusionMode = compilerOptions.useFastMath ? FloatFusionMode::Fast : FloatFusionMode::Standard;
        if (compilerOptions.positionIndependentCode.HasValue())
        {
            options.relocModel = compilerOptions.positionIndependentCode.GetValue() ? OutputRelocationModel::PIC_ : OutputRelocationModel::Static;
        }
        else
        {
            // 'auto'
            options.relocModel = compilerOptions.targetDevice.IsWindows() ? OutputRelocationModel::Static : OutputRelocationModel::PIC_;
        }

        // Other params to possibly set:
        //   FloatABIType floatABI = FloatABIType::Default;

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
            break;
        }
        default:
            throw EmitterException(EmitterError::notSupported);
        }
    }

    void IRModuleEmitter::WriteToFile(const std::string& filePath, ModuleOutputFormat format, const MachineCodeOutputOptions& options)
    {
        auto openFlags = (ModuleOutputFormat::bitcode == format || ModuleOutputFormat::objectCode == format) ? llvm::sys::fs::F_None : llvm::sys::fs::F_Text;
        std::error_code error;
        llvm::ToolOutputFile out(filePath, error, openFlags);
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
        auto compilerOptions = GetCompilerOptions();
        options.targetDevice = compilerOptions.targetDevice;
        if (compilerOptions.optimize)
        {
            options.optimizationLevel = OptimizationLevel::Aggressive;
        }
        // Other params to possibly set:
        //   bool verboseOutput = false;
        //   bool verifyModule = false;
        //   FloatABIType floatABI = FloatABIType::Default;
        //   FloatFusionMode floatFusionMode = FloatFusionMode::Standard;

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
            WriteHeader(os);
        }
        else
        {
            llvm::raw_os_ostream out(os);
            WriteToLLVMStream(out, format, options);
        }
    }

    void IRModuleEmitter::WriteToLLVMStream(llvm::raw_ostream& os, ModuleOutputFormat format, MachineCodeOutputOptions options)
    {
        const auto& params = GetCompilerOptions();

        if (options.targetDevice.triple.empty())
        {
            options.targetDevice.triple = params.targetDevice.triple;
        }

        if (options.targetDevice.cpu.empty())
        {
            options.targetDevice.cpu = params.targetDevice.cpu;
        }

        if (options.targetDevice.features.empty())
        {
            options.targetDevice.features = params.targetDevice.features;
        }

        GenerateMachineCode(os, *this, format, options);
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
        WriteModuleCppWrapper(os, *this);
    }

    //
    // Optimization
    //

    void IRModuleEmitter::Optimize(IROptimizer& optimizer)
    {
        auto compilerOptions = GetCompilerOptions();
        if (compilerOptions.optimize)
        {
            auto module = GetLLVMModule();
            for (auto& function : *module)
            {
                optimizer.OptimizeFunction(&function);
            }
            optimizer.OptimizeModule(module);
        }
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

    IRFunctionEmitter IRModuleEmitter::BeginMainDebugFunction()
    {
        DeclarePrintf();
        return BeginMainFunction();
    }

    IRDiagnosticHandler& IRModuleEmitter::GetDiagnosticHandler()
    {
        return *_diagnosticHandler;
    }

    bool IRModuleEmitter::CheckForErrors()
    {
        return llvm::verifyModule(*_llvmModule);
    }

    bool IRModuleEmitter::CheckForErrors(std::ostream& stream)
    {
        llvm::raw_os_ostream out(stream);
        return llvm::verifyModule(*_llvmModule, &out);
    }

    void IRModuleEmitter::DebugDump() const
    {
        auto module = GetLLVMModule();
        if (module)
        {
            emitters::DebugDump(module);
        }
        else
        {
            throw EmitterException(EmitterError::unexpected, "Cannot dump module because TransferOwnership has been called.");
        }
    }

    //
    // low-level LLVM-related functionality
    //

    llvm::TargetMachine* IRModuleEmitter::GetTargetMachine()
    {
        std::string error;
        auto parameters = GetCompilerOptions();

        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(parameters.targetDevice.triple, error);
        if (target == nullptr)
        {
            return nullptr;
        }

        auto relocModel = parameters.targetDevice.IsWindows() ? OutputRelocationModel::Static : OutputRelocationModel::PIC_;
        const llvm::TargetOptions options;
        const llvm::CodeModel::Model codeModel = llvm::CodeModel::Small;
        return target->createTargetMachine(parameters.targetDevice.triple,
                                           parameters.targetDevice.cpu,
                                           parameters.targetDevice.features,
                                           options,
                                           relocModel,
                                           codeModel,
                                           llvm::CodeGenOpt::Level::Default);
    }

    std::unique_ptr<llvm::Module> IRModuleEmitter::TransferOwnership()
    {
        auto result = std::move(_llvmModule);
        _llvmModule = nullptr;
        return result;
    }

    void IRModuleEmitter::SetTargetTriple(const std::string& triple)
    {
        GetLLVMModule()->setTargetTriple(triple);
    }

    const llvm::DataLayout& IRModuleEmitter::GetTargetDataLayout() const
    {
        assert(!GetLLVMModule()->getDataLayout().getStringRepresentation().empty());
        return GetLLVMModule()->getDataLayout();
    }

    void IRModuleEmitter::AddPreprocessorDefinition(const std::string& name, const std::string& value)
    {
        _preprocessorDefinitions.emplace_back(name, value);
    }

    //
    // Metadata
    //
    void IRModuleEmitter::IncludeTypeInHeader(const std::string& typeName)
    {
        InsertMetadata(c_declareTypeInHeaderTagName, { typeName });
    }

    void IRModuleEmitter::IncludeInCallbackInterface(const std::string& functionName, const std::string& nodeName)
    {
        // We shouldn't actually have to set this linkage to external because:
        // * All usages are preceded by IncludeInHeader, which already does external linkage
        // * We should be using this for just declarations that the user provides as a callback
        // That said, leaving it in for now until usage proves to be counter-productive.
        auto function = GetFunction(functionName);
        function->setLinkage(llvm::GlobalValue::LinkageTypes::ExternalLinkage);
        InsertFunctionMetadata(function, c_callbackFunctionTagName, { nodeName });
    }

    //
    // Module initialization / finalization
    //
    void IRModuleEmitter::AddInitializationFunction(LLVMFunction function, int priority, llvm::Constant* forData)
    {
        llvm::appendToGlobalCtors(*GetLLVMModule(), function, priority, forData);
    }

    void IRModuleEmitter::AddInitializationFunction(IRFunctionEmitter& function, int priority, llvm::Constant* forData)
    {
        AddInitializationFunction(function.GetFunction(), priority, forData);
    }

    void IRModuleEmitter::AddFinalizationFunction(LLVMFunction function, int priority, llvm::Constant* forData)
    {
        llvm::appendToGlobalDtors(*GetLLVMModule(), function, priority, forData);
    }

    void IRModuleEmitter::AddFinalizationFunction(IRFunctionEmitter& function, int priority, llvm::Constant* forData)
    {
        AddFinalizationFunction(function.GetFunction(), priority, forData);
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

    LLVMValue IRModuleEmitter::GetEmittedVariable(const VariableScope scope, const std::string& name)
    {
        switch (scope)
        {
        case VariableScope::literal:
            return _literals.Get(name);

        case VariableScope::global:
            return _globals.Get(name);

        case VariableScope::local:
            return GetCurrentFunction().GetEmittedVariable(scope, name);

        case VariableScope::input:
        case VariableScope::output:
            return GetCurrentFunction().GetEmittedVariable(scope, name);

        default:
            throw EmitterException(EmitterError::variableScopeNotSupported);
        }
    }

    LLVMValue IRModuleEmitter::EmitVariable(Variable& var)
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

    llvm::ConstantAggregateZero* IRModuleEmitter::ZeroInitializer(LLVMType pType)
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

        // Create a diagnostic handler to record if there was an error
        _diagnosticHandler = std::unique_ptr<IRDiagnosticHandler>(new IRDiagnosticHandler(*_llvmContext));
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

    //
    // Functions
    //

    IRModuleEmitter MakeHostModuleEmitter(const std::string moduleName)
    {
        CompilerOptions parameters;
        parameters.targetDevice.deviceName = "host";
        return { moduleName, parameters };
    }
} // namespace emitters
} // namespace ell
