#pragma once

#include "LLVMInclude.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRModuleEmitter
		{
		public:
			IRModuleEmitter(IREmitter& emitter, const std::string& name);
			IRModuleEmitter(IREmitter& emitter, std::unique_ptr<llvm::Module> pModule);

			llvm::GlobalVariable* Constant(const ValueType type, const std::string& name, double value);
			llvm::GlobalVariable* Constant(const std::string&name, const std::vector<double>& value);

			template<typename T>
			llvm::GlobalVariable* Constant(const std::string& name, T value);
			template<typename T>
			llvm::GlobalVariable* Constant(const std::string&name, const std::vector<T>& value);

			llvm::GlobalVariable* Global(const ValueType type, const std::string& name);
			llvm::GlobalVariable* Global(const ValueType type, const std::string& name, const uint64_t size);
			llvm::GlobalVariable* Global(llvm::Type* pType, const std::string& name)
			{
				return Global(name, pType, nullptr, false);
			}
			llvm::GlobalVariable* Global(const std::string& name, llvm::Type* pType, const uint64_t size);
			llvm::GlobalVariable* Global(const std::string&name, const std::vector<double>& value);
			
			template<typename T>
			llvm::GlobalVariable* Global(const std::string&name, const std::vector<T>& value);

			llvm::StructType* Struct(const std::string& name, std::initializer_list<ValueType> fields);
			
			IRFunctionEmitter AddMain();

			void DeclareFunction(const std::string& name, const ValueType returnType)
			{
				_emitter.DeclareFunction(Module(), name, returnType, nullptr);
			}
			void DeclareFunction(const std::string& name, const ValueType returnType, const ValueTypeList& args)
			{
				_emitter.DeclareFunction(Module(), name, returnType, &args);
			}
			void DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
			{
				_emitter.DeclareFunction(Module(), name, returnType, args);
			}
			void DeclareFunction(const std::string& name, llvm::FunctionType* type)
			{
				_emitter.DeclareFunction(Module(), name, type);
			}
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, bool isPublic = false)
			{
				return Function(name, returnType, nullptr, isPublic);
			}
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, const ValueTypeList& args, bool isPublic = false)
			{
				return Function(name, returnType, &args, isPublic);
			}
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, const NamedValueTypeList& args, bool isPublic = false);
			IRFunctionEmitter Function(const std::string& name, const ValueType returnType, std::initializer_list<ValueType> args, bool isPublic = false);

			llvm::Function* GetFunction(const std::string& name);

			///<summary>Return the current optimizer pipeline. It is preconfigured with some standard passes.</summary>
			llvm::FunctionPassManager* FunctionOptimizer();
			//
			// Serialization
			//
			void WriteBitcodeToFile(const std::string& filePath)
			{
				WriteToFile(filePath, true);
			}
			void WriteBitcodeToStream(std::ostream& os)
			{
				WriteToStream(os, true);
			}
			void WriteAsmToFile(const std::string& filePath)
			{
				WriteToFile(filePath, false);
			}
			void WriteAsmToStream(std::ostream& os)
			{
				WriteToStream(os, false);
			}
			//
			// To emit declarations for standard C and vector functions
			//
			void DeclarePrintf();
			void DeclareMalloc();
			void DeclareFree();
			
			///<summary>Output code to std::cout for debugging</summary>
			void Dump()
			{
				_pModule->dump();
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
			std::unique_ptr<llvm::Module> _pModule;
			std::unique_ptr<llvm::FunctionPassManager> _pOptimizer;
			IREmitter& _emitter;
			// Reusable buffers
			ValueTypeList _valueTypeList;
		};
	}
}

#include "../tcc/IRModuleEmitter.tcc"