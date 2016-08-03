////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRModuleEmitter.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "LLVMInclude.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		///<summary>Object used to emit LLVM Module level instructions</summary>
		class IRModuleEmitter
		{
		public:
			///<summary>Construct a new module emitter to output a new named module</summary>
			IRModuleEmitter(IREmitter& emitter, const std::string& moduleName);
			///<summary>Construct an emitter for the given module</summary>
			IRModuleEmitter(IREmitter& emitter, std::unique_ptr<llvm::Module> pModule);

			///<summary>Emit a named, module scoped Constant double</summary>
			llvm::GlobalVariable* Constant(const ValueType type, const std::string& name, double value);
			///<summary>Emit a named, module scoped Constant string</summary>
			llvm::GlobalVariable* Constant(const std::string&name, const std::vector<double>& value);

			///<summary>Generic template for emitting scalar constant</summary>
			template<typename T>
			llvm::GlobalVariable* Constant(const std::string& name, T value);
			///<summary>Generic template for emitting array constants</summary>
			template<typename T>
			llvm::GlobalVariable* Constant(const std::string&name, const std::vector<T>& value);

			///<summary>Emit a named scalar global of the given type</summary>
			llvm::GlobalVariable* Global(const ValueType type, const std::string& name);
			///<summary>Emit a named global array of the given type and size</summary>
			llvm::GlobalVariable* Global(const ValueType type, const std::string& name, const uint64_t size);
			///<summary>Emit a named global scalar of the given type</summary>
			llvm::GlobalVariable* Global(llvm::Type* pType, const std::string& name)
			{
				return Global(name, pType, nullptr, false);
			}
			///<summary>Emit a named global array of the given type and size</summary>
			llvm::GlobalVariable* Global(const std::string& name, llvm::Type* pType, const uint64_t size);
			///<summary>Emit a named global array of doubles</summary>
			llvm::GlobalVariable* Global(const std::string&name, const std::vector<double>& value);			
			///<summary>Generic global emitter</summary>
			template<typename T>
			llvm::GlobalVariable* Global(const std::string&name, const std::vector<T>& value);

			///<summary>Define a module scoped STRUCTURE with the given fields</summary>
			llvm::StructType* Struct(const std::string& name, std::initializer_list<ValueType> fields);
			
			///<summary>Emit a "main" function...the entry point to an LLVM program</summary>
			IRFunctionEmitter AddMain();

			///<summary>Declare an external function with the given return type</summary>
			void DeclareFunction(const std::string& name, const ValueType returnType)
			{
				_emitter.DeclareFunction(Module(), name, returnType, nullptr);
			}
			///<summary>Declare an external function with the given return type and args</summary>
			void DeclareFunction(const std::string& name, const ValueType returnType, const ValueTypeList& args)
			{
				_emitter.DeclareFunction(Module(), name, returnType, &args);
			}
			///<summary>Declare an external function with the given return type and named args</summary>
			void DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
			{
				_emitter.DeclareFunction(Module(), name, returnType, args);
			}
			///<summary>Declare an external function with the given function signature</summary>
			void DeclareFunction(const std::string& name, llvm::FunctionType* type)
			{
				_emitter.DeclareFunction(Module(), name, type);
			}

			///<summary>Emit a function with the given return type</summary>
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, bool isPublic = false)
			{
				return Function(name, returnType, nullptr, isPublic);
			}
			///<summary>Emit a function with the given return type and arguments</summary>
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, const ValueTypeList& args, bool isPublic = false)
			{
				return Function(name, returnType, &args, isPublic);
			}
			///<summary>Emit a function with the given return type and named args</summary>
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, const NamedValueTypeList& args, bool isPublic = false);
			///<summary>Emit a function with the given return type and args</summary>
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, std::initializer_list<ValueType> args, bool isPublic = false);

			///<summary>Get an emitted or declared function with the given name</summary>
			llvm::Function* GetFunction(const std::string& name);
			///<summary>Get an LLVM intrinsic function with the given name and signature</summary>
			llvm::Function* GetIntrinsic(llvm::Intrinsic::ID id, std::initializer_list<ValueType> args);

			///<summary>Return the current optimizer pipeline. It is preconfigured with some standard passes.</summary>
			llvm::FunctionPassManager* FunctionOptimizer();
			//
			// Serialization
			//
			///<summary>Write this module as IR bitcode to this file</summary>
			void WriteBitcodeToFile(const std::string& filePath)
			{
				WriteToFile(filePath, true);
			}
			///<summary>Write this module as IR bitcode to this stream</summary>
			void WriteBitcodeToStream(std::ostream& os)
			{
				WriteToStream(os, true);
			}
			///<summary>Write this module as IR text assembly to this file</summary>
			void WriteAsmToFile(const std::string& filePath)
			{
				WriteToFile(filePath, false);
			}
			///<summary>Write this module as IR text assembly to this stream</summary>
			void WriteAsmToStream(std::ostream& os)
			{
				WriteToStream(os, false);
			}

			//
			// To emit declarations for standard C and vector functions - LLVM supports these
			// You must declare these before you can call them
			//
			///<summary>Declare extern printf</summary>
			void DeclarePrintf();
			///<summary>Declare extern malloc</summary>
			void DeclareMalloc();
			///<summary>Declare extern free</summary>
			void DeclareFree();
			
			///<summary>Output code to std::cout for debugging</summary>
			void Dump()
			{
				_pModule->dump();
			}

			///<summary>For direct access to LLVM APIs</summary>
			IREmitter& Emitter()
			{
				return _emitter;
			}
			///<summary>For direct access to LLVM APIs</summary>
			llvm::Module* Module() const
			{
				return _pModule.get();
			}

		private:

			llvm::GlobalVariable* Global(const std::string& name, llvm::Type *pType, llvm::Constant* pInitial, bool isConst);
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, const ValueTypeList* pArgs, bool isPublic);
			void BeginFunction(llvm::Function* pfn);
			void WriteToFile(const std::string& filePath, bool isBitCode);
			void WriteToStream(std::ostream& os, bool isBitCode);
			llvm::Function::LinkageTypes Linkage(bool isPublic);
			llvm::ConstantAggregateZero* InitializeArray(llvm::ArrayType* pType);

		private:
			std::unique_ptr<llvm::Module> _pModule;	// The LLVM Module being emitted
			std::unique_ptr<llvm::FunctionPassManager> _pOptimizer;
			IREmitter& _emitter;
			// Reusable buffers
			ValueTypeList _valueTypeList;
		};
	}
}

#include "../tcc/IRModuleEmitter.tcc"