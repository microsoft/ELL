////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModuleEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterException.h"
#include "IRAssemblyEmitter.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRRuntime.h"
#include "LLVMInclude.h"
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

    ///<summary> Object used to emit LLVM Module level instructions. </summary>
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

        //
        // Getting state
        //

        /// <summary> The current function being emitted </summary>
        IRFunctionEmitter& GetCurrentFunction();

        /// <summary> Returns the current block being emitted into </summary>
        IRBlockRegion* GetCurrentRegion() { return GetCurrentFunction().GetCurrentRegion(); } 

        /// <summary> Returns the runtime object that manages functions </summary>
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

        /// <summary> Begins an IR function and directs subsequent commands to it </summary>
        ///
        /// <param name="functionName"> The name of the function </param>
        /// <param name="args"> The arguments to the function </param>
        virtual void BeginTopLevelFunction(const std::string& functionName, NamedVariableTypeList& args) override; // TODO: add return type

        /// <summary> Ends the current function </summary>
        virtual void EndTopLevelFunction() override;


        // TODO: replace BeginTopLevelFunction with this
        IRFunctionEmitter& BeginFunction(const std::string& functionName, VariableType returnType, NamedVariableTypeList& args);

        /// <summary> Ends the current function </summary>
        void EndFunction();

        //
        // Variable management
        //

        /// <summary> Ensure that the given variable has been declared in IR </summary>
        llvm::Value* EnsureEmitted(Variable& var);

        /// <summary> Ensure that the given variable is loaded into a register </summary>
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

        /// <summary> Emit a named, module scoped constant string. </summary>
        ///
        /// <param name="name"> The name of the string. </param>
        /// <param name="value"> The value of the string. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        llvm::GlobalVariable* Constant(const std::string& name, const std::vector<double>& value);

        ///<summary> Emit a named, module scoped constant of a template type.</summary>
        ///
        /// <param name="name"> The name of the constant. </param>
        /// <param name="value"> The value of the constant. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        template <typename ValueType>
        llvm::GlobalVariable* Constant(const std::string& name, ValueType value);

        ///<summary> Emit a named, module scoped array constant of a template type.</summary>
        ///
        /// <typeparam name="ValueType"> Type of each array entry. </typeparam>
        /// <param name="name"> The array constant name. </param>
        /// <param name="value"> The array constant value. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the constant. </returns>
        template <typename ValueType>
        llvm::GlobalVariable* Constant(const std::string& name, const std::vector<ValueType>& value);

        /// <summary> Emit a named global variable of the given type. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="name"> The name of the variable. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(VariableType type, const std::string& name);

        /// <summary> Emit a named global array of the given type and size. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="name"> The name of the variable. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(VariableType type, const std::string& name, const size_t size);

        /// <summary> Emit a named global variable of the given type. </summary>
        ///
        /// <param name="pType"> Pointer to the runtime value that contains the variable type. </param>
        /// <param name="name"> The name of the variable. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(llvm::Type* pType, const std::string& name);

        /// <summary> Emit a named global array of the given type and size. </summary>
        ///
        /// <param name="name"> The name of the variable. </param>
        /// <param name="pType"> Pointer to the runtime value that contains the variable type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(const std::string& name, llvm::Type* pType, const size_t size);

        /// <summary> Emit a named global array of doubles. </summary>
        ///
        /// <param name="name"> The name of the variable. </param>
        /// <param name="value"> The value of the array. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        llvm::GlobalVariable* Global(const std::string& name, const std::vector<double>& value);

        ///<summary> Emit a named, module scoped array constant of a template type.</summary>
        ///
        /// <typeparam name="ValueType"> Type of each array entry. </typeparam>
        /// <param name="value"> The value of the array. </param>
        /// <param name="value"> The value of the array. </param>
        ///
        /// <returns> Pointer to the llvm::GlobalVariable that represents the variable. </returns>
        template <typename ValueType>
        llvm::GlobalVariable* Global(const std::string& name, const std::vector<ValueType>& value);

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
        /// <param name="returnType"> The function return type. </param>
        void DeclareFunction(const std::string& name, llvm::FunctionType* returnType);

        /// <summary> Emit a function with the given return type. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        /// <param name="isPublic"> true if the function is public. </param>
        ///
        /// <returns> An IRFunctionEmitter. </returns>
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, bool isPublic = false);

        /// <summary> Emit a function with the given return type and arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        /// <param name="arguments"> The function arguments. </param>
        /// <param name="isPublic"> true if the function is public. </param>
        ///
        /// <returns> An IRFunctionEmitter. </returns>
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, const ValueTypeList& arguments, bool isPublic = false);

        /// <summary> Emit a function with the given return type and named arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        /// <param name="arguments"> The named function arguments. </param>
        /// <param name="isPublic"> true if the function is public. </param>
        ///
        /// <returns> An IRFunctionEmitter. </returns>
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments, bool isPublic = false);

        /// <summary> Emit a function with the given return type and arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> The function return type. </param>
        /// <param name="arguments"> The named function arguments. </param>
        /// <param name="isPublic"> true if the function is public. </param>
        ///
        /// <returns> An IRFunctionEmitter. </returns>
        IRFunctionEmitter Function(const std::string& name, VariableType returnType, std::initializer_list<VariableType> arguments, bool isPublic = false);

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
        llvm::Function* GetIntrinsic(llvm::Intrinsic::ID id, std::initializer_list<VariableType> arguments);

        //
        // Types
        //

        /// <summary> Emit a module scoped struct with the given fields. </summary>
        ///
        /// <param name="name"> The struct name. </param>
        /// <param name="fields"> The struct fields. </param>
        ///
        /// <returns> Pointer to the llvm::StructType that represents the emitted structure. </returns>
        llvm::StructType* Struct(const std::string& name, std::initializer_list<VariableType> fields);

        //
        // Code output / input
        //

        /// <summary> Output the compiled module to the given file, using file extension to determine output format </summary>
        ///
        /// <param name="filePath"> The path of the file to write to </param>
        using ModuleEmitter::WriteToFile;

        /// <summary> Output the compiled module to an output file with the given format </summary>
        ///
        /// <param name="filePath"> Full pathname of the file. </param>
        /// <param name="format"> The format of the output </param>
        void WriteToFile(const std::string& filePath, ModuleOutputFormat format);

        /// <summary> Output the compiled module to an output file with the given format </summary>
        ///
        /// <param name="filePath"> Full pathname of the file. </param>
        /// <param name="format"> The format of the output </param>
        /// <param name="options"> Options to control how machine code is generated during output </params>
        void WriteToFile(const std::string& filePath, ModuleOutputFormat format, const MachineCodeOutputOptions& options);

        /// <summary> Output the compiled module to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format of the output </param>
        virtual void WriteToStream(std::ostream& stream, ModuleOutputFormat format) override;

        /// <summary> Output the compiled module to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format of the output </param>
        /// <param name="options"> Options to control how machine code is generated during output </params>
        void WriteToStream(std::ostream& stream, ModuleOutputFormat format, const MachineCodeOutputOptions& options);

        /// <summary> Load assembly text into this module. </summary>
        ///
        /// <param name="text"> The assembly text. </param>
        void LoadAssembly(const std::string& text);

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

        /// <summary> Emit a "main" function, the entry point to an LLVM program. </summary>
        ///
        /// <returns> An IRFunctionEmitter. </returns>
        IRFunctionEmitter AddMain();

        /// <summary>Emit declaration of extern printf </summary>
        void DeclarePrintf();

        /// <summary> Emit declaration of extern malloc </summary>
        void DeclareMalloc();

        /// <summary> Emit declaration of extern free </summary>
        void DeclareFree();

        /// <summary> Add a main function into which you will inject debugging code </summary>
        IRFunctionEmitter AddMainDebug();

        /// <summary> Emit LLVM IR to std::out for debugging </summary>
        void DebugDump();

        //
        // low-level LLVM-related functionality
        //

        /// <summary> Gets an owning pointer to the underlying llvm::Module. </summary>
        ///
        /// <returns> Unique pointer to the underlying llvm::Module. </returns>
        /// <remarks> After calling this, the module will not be valid </remarks>
        std::unique_ptr<llvm::Module> TransferOwnership();

        /// <summary> Gets a pointer to the underlying llvm::Module. </summary>
        ///
        /// <returns> Pointer to the underlying llvm::Module. </returns>
        llvm::Module* GetLLVMModule() const { return _pModule.get(); }

        /// <summary> Can this module emitter still be used to add functions to the module? </summary>
        ///
        /// <returns> true if active, false if not. </returns>
        bool IsActive() const { return (_pModule != nullptr); }

        /// <summary> Gets a reference to the underlying llvm context. </summary>
        ///
        /// <returns> Reference to the underlying llvm context. </returns>
        llvm::LLVMContext& GetLLVMContext() { return _llvmContext; }

    private:
        friend class IRFunctionEmitter;

        //
        // Internal variable and function creation implementation
        //

        /// <summary> Gets an emitted variable with the given name and scope </summary>
        llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);

        /// <summary> Emit IR for a variable </summary>
        llvm::Value* EmitVariable(Variable& var);

        // Templated version implementing above
        template <typename T>
        llvm::Value* EmitVariable(Variable& var);

        /// <summary> Emit IR for a scalar variable </summary>
        template <typename T>
        llvm::Value* EmitScalar(Variable& var);

        /// <summary> Emit IR for a vector variable </summary>
        template <typename T>
        llvm::Value* EmitVector(Variable& var);

        /// <summary> Emit IR for a constant </summary>
        template <typename T>
        llvm::Value* EmitLiteral(LiteralVariable<T>& var);

        /// <summary> Emit IR for a Global Scalar with an initial value </summary>
        template <typename T>
        llvm::Value* EmitGlobal(InitializedScalarVariable<T>& var);

        /// <summary> Emit IR for a stack scalar variable </summary>
        template <typename T>
        llvm::Value* EmitLocal(ScalarVariable<T>& var);

        /// <summary> Emit IR for a stack scalar variable with an initial value </summary>
        template <typename T>
        llvm::Value* EmitLocal(InitializedScalarVariable<T>& var);

        /// <summary> Emit IR for a Constant Vector </summary>
        template <typename T>
        llvm::Value* EmitLiteralVector(LiteralVectorVariable<T>& var);

        /// <summary> Emit IR for a Global Vector. Global is initialized to zero </summary>
        template <typename T>
        llvm::Value* EmitGlobalVector(VectorVariable<T>& var);

        /// <summary> Emit IR for a Global Vector with initial values </summary>
        template <typename T>
        llvm::Value* EmitGlobalVector(InitializedVectorVariable<T>& var);

        /// <summary> Emit IR for a reference to an element in a vector </summary>
        template <typename T>
        llvm::Value* EmitRef(VectorElementVariable<T>& var);

        // Actual code output implementation
        void WriteToLLVMStream(llvm::raw_ostream& stream, ModuleOutputFormat format, const MachineCodeOutputOptions& options);

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
        std::stack<std::pair<IRFunctionEmitter, llvm::BasicBlock*>> _functionStack;

        IRVariableTable _literals; // Symbol table - name to literals
        IRVariableTable _globals; // Symbol table - name to global variables
        IRRuntime _runtime; // Manages emission of runtime functions

        std::unique_ptr<llvm::Module> _pModule; // The LLVM Module being emitted

        ValueTypeList _valueTypeList;
    };
}
}

#include "../tcc/IRModuleEmitter.tcc"
