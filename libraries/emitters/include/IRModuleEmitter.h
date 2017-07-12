////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModuleEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterException.h"
#include "IRAssemblyWriter.h"
#include "IRDiagnosticHandler.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRRuntime.h"
#include "ModuleEmitter.h"
#include "ScalarVariable.h"
#include "VectorVariable.h"

// stl
#include <initializer_list>
#include <iosfwd>
#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRExecutionEngine;

    /// <summary> Object used to emit LLVM Module level instructions. </summary>
    class IRModuleEmitter : public ModuleEmitter
    {
    public:
        /// <summary> Construct a new module emitter to output a new named module. </summary>
        ///
        /// <param name="emitter"> An IREmitter. </param>
        /// <param name="moduleName"> Name of the module. </param>
        IRModuleEmitter(const std::string& moduleName);

        /// <summary> Construct an emitter for the given module. </summary>
        ///
        /// <param name="emitter"> An IREmitter. </param>
        /// <param name="pModule"> Unique pointer to the llvm module. </param>
        IRModuleEmitter(std::unique_ptr<llvm::Module> pModule);

        IRModuleEmitter(IRModuleEmitter&& other) = default;
        virtual ~IRModuleEmitter() = default;

        //
        // Properties of the module
        //

        /// <summary> Returns the module's name. </summary>
        std::string GetModuleName() const { return _pModule->getName(); }

        //
        // Getting state
        //

        /// <summary> The current function being emitted. </summary>
        IRFunctionEmitter& GetCurrentFunction();

        /// <summary> Returns the current block being emitted into. </summary>
        IRBlockRegion* GetCurrentRegion() { return GetCurrentFunction().GetCurrentRegion(); }

        /// <summary> Returns the runtime object that manages functions. </summary>
        IRRuntime& GetRuntime() { return _runtime; }

        /// <summary> Gets a reference to the underlying IREmitter. </summary>
        ///
        /// <returns> Reference to the underlying IREmitter. </returns>
        IREmitter& GetIREmitter() { return _emitter; }

        /// <summary> Can this module emitter still be used to add functions to the module? </summary>
        ///
        /// <returns> true if active, false if not. </returns>
        bool IsValid() const { return (_pModule != nullptr); }

        //
        // Creating functions
        //

        /// <summary> Begins a function and directs subsequent commands to it. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="args"> The arguments to the function. </param>
        virtual void BeginMapPredictFunction(const std::string& functionName, NamedVariableTypeList& args) override;

        /// <summary> Ends the current model prediction function. </summary>
        virtual void EndMapPredictFunction() override { EndFunction(); }

        /// <summary> Begins an IR function with no arguments and directs subsequent commands to it. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="returnType"> The return type of the function. </param>
        IRFunctionEmitter& BeginFunction(const std::string& functionName, VariableType returnType);

        /// <summary> Begins an IR function and directs subsequent commands to it. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="returnType"> The return type of the function. </param>
        /// <param name="args"> The arguments to the function. </param>
        IRFunctionEmitter& BeginFunction(const std::string& functionName, VariableType returnType, const ValueTypeList& args);

        /// <summary> Begins an IR function and directs subsequent commands to it. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="returnType"> The return type of the function. </param>
        /// <param name="args"> The arguments to the function. </param>
        IRFunctionEmitter& BeginFunction(const std::string& functionName, VariableType returnType, const NamedVariableTypeList& args);

        /// <summary> Begins an IR function and directs subsequent commands to it. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="returnType"> The return type of the function. </param>
        /// <param name="argsTypes"> The argument types for the function. </param>
        IRFunctionEmitter& BeginFunction(const std::string& functionName, llvm::Type* returnType, const std::vector<llvm::Type*>& argTypes);

        /// <summary> Emit a "main" function, the entry point to an LLVM program. </summary>
        ///
        /// <returns> An IRFunctionEmitter. </returns>
        IRFunctionEmitter BeginMainFunction();

        /// <summary> Ends the current function. </summary>
        void EndFunction();

        /// <summary> Ends the current function with a return value. </summary>
        ///
        /// <param name="return"> The value the function returns. </param>
        void EndFunction(llvm::Value* pReturn);

        //
        // Variable management
        //

        /// <summary> Ensure that the given variable has been declared in IR. </summary>
        llvm::Value* EnsureEmitted(Variable& var);

        /// <summary> Ensure that the given variable is loaded into a register. </summary>
        llvm::Value* LoadVariable(Variable& var);

        /// <summary> Updates the value at a given offset of the given variable. Checks for index out of range etc. </summary>
        void SetVariable(Variable& var, llvm::Value* pDest, int offset, llvm::Value* pValue);

        //
        // Variable and Constant creation
        //

        /// <summary> Emit a named, module scoped constant. </summary>
        ///
        /// <param name="type"> The type of the constant. </param>
        /// <param name="name"> The name of the constant. </param>
        /// <param name="value"> The value of the contant. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        llvm::GlobalVariable* Constant(VariableType type, const std::string& name, double value);

        /// <summary> Emit a named, module scoped constant of a template type. </summary>
        ///
        /// <param name="name"> The name of the constant. </param>
        /// <param name="value"> The value of the constant. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        template <typename ValueType>
        llvm::GlobalVariable* Constant(const std::string& name, ValueType value);

        /// <summary> Emit a named, module scoped constant vector. </summary>
        ///
        /// <param name="name"> The name of the vector. </param>
        /// <param name="value"> The value of the bector. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        llvm::GlobalVariable* ConstantArray(const std::string& name, const std::vector<double>& value);

        /// <summary> Emit a named, module scoped array constant of a template type. </summary>
        ///
        /// <typeparam name="ValueType"> Type of each array entry. </typeparam>
        /// <param name="name"> The array constant name. </param>
        /// <param name="value"> The array constant value. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        template <typename ValueType>
        llvm::GlobalVariable* ConstantArray(const std::string& name, const std::vector<ValueType>& value);

        /// <summary> Emit a named global variable of the given type. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="name"> The name of the variable. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(VariableType type, const std::string& name);

        /// <summary> Emit a named global variable of the given type. </summary>
        ///
        /// <param name="pType"> Pointer to the runtime value that contains the variable type. </param>
        /// <param name="name"> The name of the variable. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(llvm::Type* pType, const std::string& name);

        /// <summary> Emit a named global array of the given type and size. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="name"> The name of the variable. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* GlobalArray(VariableType type, const std::string& name, const size_t size);

        /// <summary> Emit a named global array of the given type and size. </summary>
        ///
        /// <param name="name"> The name of the variable. </param>
        /// <param name="pType"> Pointer to the runtime value that contains the variable type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* GlobalArray(const std::string& name, llvm::Type* pType, const size_t size);

        /// <summary> Emit a named global array of doubles. </summary>
        ///
        /// <param name="name"> The name of the variable. </param>
        /// <param name="value"> The value of the array. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* GlobalArray(const std::string& name, const std::vector<double>& value);

        /// <summary> Emit a named, module scoped array constant of a template type. </summary>
        ///
        /// <typeparam name="ValueType"> Type of each array entry. </typeparam>
        /// <param name="value"> The value of the array. </param>
        /// <param name="value"> The value of the array. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        template <typename ValueType>
        llvm::GlobalVariable* GlobalArray(const std::string& name, const std::vector<ValueType>& value);

        //
        // Functions
        //

        /// <summary> Emit a declaration of an external function with the given return type. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        void DeclareFunction(const std::string& name, VariableType returnType);

        /// <summary> Emit a declaration of an external function with the given return type and arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        /// <param name="arguments"> The function arguments. </param>
        void DeclareFunction(const std::string& name, VariableType returnType, const ValueTypeList& arguments);

        /// <summary>
        /// Emit a declaration of an external function with the given return type and named arguments.
        /// </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        /// <param name="arguments"> The named function arguments. </param>
        void DeclareFunction(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments);

        /// <summary> Emit a declaration of an external function with the given function signature. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="functionType"> The function type. </param>
        void DeclareFunction(const std::string& name, llvm::FunctionType* functionType);

        /// <summary> Check if a given function name exists. </summary>
        ///
        /// <param name="name"> The function name. </param>
        ///
        /// <returns> True if the function name exists, false if not. </returns>
        bool HasFunction(const std::string& name);

        /// <summary> Get an emitted or declared function with the given name. </summary>
        ///
        /// <param name="name"> The function name. </param>
        ///
        /// <returns> Pointer to an llvm::Function that represents the requested function. </returns>
        llvm::Function* GetFunction(const std::string& name);

        /// <summary> Get an LLVM intrinsic function with the given id and signature. </summary>
        ///
        /// <param name="id"> The intrinsic function identifier. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to an llvm::Function that represents the requested function. </returns>
        llvm::Function* GetIntrinsic(llvm::Intrinsic::ID id, const std::initializer_list<VariableType>& arguments);

        //
        // Types
        //

        /// <summary> Emit a module scoped struct with the given fields. </summary>
        ///
        /// <param name="name"> The struct name. </param>
        /// <param name="fields"> The struct fields. </param>
        ///
        /// <returns> Pointer to the llvm::StructType that represents the emitted structure. </returns>
        llvm::StructType* Struct(const std::string& name, const std::initializer_list<VariableType>& fields);

        /// <summary> Emit a module scoped struct with the given fields. </summary>
        ///
        /// <param name="name"> The struct name. </param>
        /// <param name="fields"> The struct fields. </param>
        ///
        /// <returns> Pointer to the llvm::StructType that represents the emitted structure. </returns>
        llvm::StructType* Struct(const std::string& name, const std::vector<VariableType>& fields);

        //
        // Code annotation
        //

        /// <summary> Indicates if the given function has any associated comments. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        ///
        /// <returns> `true` if the function has any comments associated with it. </returns>
        virtual bool HasFunctionComments(const std::string& functionName) override;

        /// <summary> Get the comments associated with the given function. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        ///
        /// <returns> The comments for the function, as a vector of strings. </returns>
        virtual std::vector<std::string> GetFunctionComments(const std::string& functionName) override;

        /// <summary> Associates some comment text with the given function. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="comments"> The comments for the function. </param>
        virtual void SetFunctionComments(const std::string& functionName, const std::vector<std::string>& comments) override;

        /// <summary> Gets any preprocessor definitions set for the module. </summary>
        ///
        /// <returns> The preprocessor definitions set for the module, as a vector of (name,value) pairs. </returns>
        const std::vector<std::pair<std::string, std::string>>& GetPreprocessorDefinitions() const { return _preprocessorDefinitions; };

        /// <summary> Adds a preprocessor definition to the header file. </summary>
        ///
        /// <param name="name"> The name of the preprocessor symbol to define. </param>
        /// <param name="value"> The value to set for the symbol. </param>
        void AddPreprocessorDefinition(const std::string& name, const std::string& value);

        /// <summary> Indicates if the module or given function has the associated metadata. </summary>
        ///
        /// <param name="functionName"> The name of the function for function-level metadata, or empty string for the module. </param>
        /// <param name="tag"> The metadata tag. </param>
        ///
        /// <returns> `true` if the function has the metadata associated with it. </returns>
        virtual bool HasMetadata(const std::string& functionName, const std::string& tag) override;

        /// <summary> Gets the metadata associated with the module or given function. </summary>
        ///
        /// <param name="functionName"> The name of the function for function-level metadata, or empty string for the module. </param>
        /// <param name="tag"> The metadata tag. </param>
        ///
        /// <returns> The metadata values, as a vector of strings. </returns>
        virtual std::vector<std::string> GetMetadata(const std::string& functionName, const std::string& tag) override;

        /// <summary> Associates metadata with the module or given function. </summary>
        ///
        /// <param name="functionName"> The name of the function for function-level metadata, or empty string for the module. </param>
        /// <param name="tag"> The metadata tag. </param>
        /// <param name="content"> Optional metadata value. </param>
        /// <remarks> To insert well-known metadata, prefer the "IncludeInXXX" metadata methods. </remarks>
        virtual void InsertMetadata(const std::string& functionName, const std::string& tag, const std::string& value = "") override;

        //
        // Code output / input
        //

        /// <summary> Output the compiled module to the given file, using file extension to determine output format. </summary>
        ///
        /// <param name="filePath"> The path of the file to write to. </param>
        using ModuleEmitter::WriteToFile;

        /// <summary> Output the compiled module to an output file with the given format. </summary>
        ///
        /// <param name="filePath"> Full pathname of the file. </param>
        /// <param name="format"> The format of the output. </param>
        virtual void WriteToFile(const std::string& filePath, ModuleOutputFormat format) override;

        /// <summary> Output the compiled module to an output file with the given format. </summary>
        ///
        /// <param name="filePath"> Full pathname of the file. </param>
        /// <param name="format"> The format of the output. </param>
        /// <param name="options"> Options to control how machine code is generated during output. </params>
        void WriteToFile(const std::string& filePath, ModuleOutputFormat format, const MachineCodeOutputOptions& options);

        /// <summary> Output the compiled module to an output stream with the given format. </summary>
        ///
        /// <param name="stream"> The stream to write to. </param>
        /// <param name="format"> The format of the output. </param>
        virtual void WriteToStream(std::ostream& stream, ModuleOutputFormat format) override;

        /// <summary> Output the compiled module to an output stream with the given format. </summary>
        ///
        /// <param name="stream"> The stream to write to. </param>
        /// <param name="format"> The format of the output. </param>
        /// <param name="options"> Options to control how machine code is generated during output. </params>
        void WriteToStream(std::ostream& stream, ModuleOutputFormat format, const MachineCodeOutputOptions& options);

        /// <summary> Load LLVM IR text into this module. </summary>
        ///
        /// <param name="text"> The IR text. </param>
        void LoadIR(const std::string& text);

        //
        // Optimization
        //

        /// <summary> Run standard module optimization passes. </summary>
        void Optimize();

        /// <summary> Optimize this module using the given optimizer. </summary>
        ///
        /// <param name="optimizer"> The optimizer. </param>
        void Optimize(IRModuleOptimizer& optimizer);

        /// <summary>
        /// Set the target machine and arch for this module. This aids the system in optimizations and
        /// Jitting etc.
        /// </summary>
        ///
        /// <param name="pMachine"> Pointer to a llvm::TargetMachine object that describes the target machine. </param>
        void SetTargetMachine(llvm::TargetMachine* pMachine);

        //
        // Helpers, standard C Runtime functions, and debug support
        //

        /// <summary>Emit declaration of extern printf. </summary>
        void DeclarePrintf();

        /// <summary> Emit declaration of extern malloc. </summary>
        void DeclareMalloc();

        /// <summary> Emit declaration of extern free. </summary>
        void DeclareFree();

        /// <summary> Emit declaration of GetXXClockMilliseconds. </summary>
        ///
        /// <typeparam name="ClockType"> The clock type. </typeparam>
        template <typename ClockType>
        void DeclareGetClockMilliseconds();

        /// <summary> Add a main function into which you will inject debugging code. </summary>
        IRFunctionEmitter BeginMainDebugFunction();

        /// <summary> Get the diagnostic handler. </summary>
        IRDiagnosticHandler& GetDiagnosticHandler();

        /// <summary> Emit LLVM IR to std::out for debugging. </summary>
        void DebugDump();

        //
        // low-level LLVM-related functionality
        //

        /// <summary> Gets an owning pointer to the underlying llvm::Module. </summary>
        ///
        /// <returns> Unique pointer to the underlying llvm::Module. </returns>
        /// <remarks> After calling this, the module will not be valid. </remarks>
        std::unique_ptr<llvm::Module> TransferOwnership();

        /// <summary> Gets a pointer to the underlying llvm::Module. </summary>
        ///
        /// <returns> Pointer to the underlying llvm::Module. </returns>
        llvm::Module* GetLLVMModule() const { return _pModule.get(); }

        /// <summary> Sets the LLVM triple for the current module. </summary>
        ///
        /// <param name="triple"> The triple representing the desired machine configuration. </param>
        void SetTargetTriple(const std::string& triple);

        /// <summary> Sets the LLVM data layout string for the current module. </summary>
        ///
        /// <param name="triple"> The data layout string representing the desired machine configuration. </param>
        void SetTargetDataLayout(const std::string& dataLayout);

        /// <summary> Can this module emitter still be used to add functions to the module? </summary>
        ///
        /// <returns> true if active, false if not. </returns>
        bool IsActive() const { return (_pModule != nullptr); }

        /// <summary> Gets a reference to the underlying llvm context. </summary>
        ///
        /// <returns> Reference to the underlying llvm context. </returns>
        llvm::LLVMContext& GetLLVMContext() { return _llvmContext; }

        //
        // Metadata
        //

        /// <summary> Tags a function to be declared in a C/C++ header. </summary>
        ///
        /// <param name="functionName"> The function name. </param>
        void IncludeInHeader(const std::string& functionName);

        /// <summary> Tags a type to be declared in a C/C++ header. </summary>
        ///
        /// <param name="typeName"> The name of the type. </param>
        void IncludeTypeInHeader(const std::string& typeName);

        /// <summary> Tags a callback function to be included in the SWIG interface. </summary>
        ///
        /// <param name="functionName"> The function name. </param>
        /// <param name="nodeName"> The node name. </param>
        void IncludeInCallbackInterface(const std::string& functionName, const std::string& nodeName);

    private:
        friend class IRFunctionEmitter;

        //
        // Internal variable and function creation implementation
        //

        /// Gets an emitted variable with the given name and scope.
        llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);

        /// Emit IR for a variable.
        llvm::Value* EmitVariable(Variable& var);

        // Templated version implementing above
        template <typename T>
        llvm::Value* EmitVariable(Variable& var);

        /// Emit IR for a scalar variable.
        template <typename T>
        llvm::Value* EmitScalar(Variable& var);

        /// Emit IR for a vector variable.
        template <typename T>
        llvm::Value* EmitVector(Variable& var);

        /// Emit IR for a constant.
        template <typename T>
        llvm::Value* EmitLiteral(LiteralVariable<T>& var);

        /// Emit IR for a Global scalar with an initial value.
        template <typename T>
        llvm::Value* EmitGlobal(InitializedScalarVariable<T>& var);

        /// Emit IR for a stack scalar variable.
        template <typename T>
        llvm::Value* EmitLocal(ScalarVariable<T>& var);

        /// Emit IR for a stack scalar variable with an initial value.
        template <typename T>
        llvm::Value* EmitLocal(InitializedScalarVariable<T>& var);

        /// Emit IR for a constant vector.
        template <typename T>
        llvm::Value* EmitLiteralVector(LiteralVectorVariable<T>& var);

        /// Emit IR for a global vector. Global is initialized to zero.
        template <typename T>
        llvm::Value* EmitGlobalVector(VectorVariable<T>& var);

        /// Emit IR for a global vector with initial values.
        template <typename T>
        llvm::Value* EmitGlobalVector(InitializedVectorVariable<T>& var);

        /// Emit IR for a reference to an element in a vector.
        template <typename T>
        llvm::Value* EmitRef(VectorElementVariable<T>& var);

        IRFunctionEmitter Function(const std::string& name, VariableType returnType, bool isPublic = false);
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, const ValueTypeList& arguments, bool isPublic = false);
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments, bool isPublic = false);
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, const std::initializer_list<VariableType>& arguments, bool isPublic = false);
        IRFunctionEmitter Function(const std::string& name, llvm::Type* returnType, const std::vector<llvm::Type*>& argTypes, bool isPublic = false);

        // Actual code output implementations
        void WriteHeader(std::ostream& stream);
        void WriteToLLVMStream(llvm::raw_ostream& stream, ModuleOutputFormat format, MachineCodeOutputOptions options);

        //
        // Lower-level internal functions
        //
        llvm::GlobalVariable* Global(const std::string& name, llvm::Type* pType, llvm::Constant* pInitial, bool isConst);
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, const ValueTypeList* pArguments, bool isPublic);
        llvm::Function::LinkageTypes Linkage(bool isPublic);
        llvm::ConstantAggregateZero* InitializeArray(llvm::ArrayType* pType);

        //
        // LLVM global state management
        //
        static void InitializeLLVM();
        static llvm::PassRegistry* InitializeGlobalPassRegistry();

        //
        // Data members
        //
        llvm::LLVMContext& _llvmContext; // LLVM global context
        IREmitter _emitter;
        std::stack<std::pair<IRFunctionEmitter, llvm::IRBuilder<>::InsertPoint>> _functionStack; // contains the location we were emitting code into when we paused to emit a new function

        IRVariableTable _literals; // Symbol table - name to literals
        IRVariableTable _globals; // Symbol table - name to global variables
        IRRuntime _runtime; // Manages emission of runtime functions

        std::unique_ptr<llvm::Module> _pModule; // The LLVM Module being emitted

        // Info to modify how code is written out
        std::map<std::string, std::vector<std::string>> _functionComments;
        std::vector<std::pair<std::string, std::string>> _preprocessorDefinitions;
    };
}
}

#include "../tcc/IRModuleEmitter.tcc"
