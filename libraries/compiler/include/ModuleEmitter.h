#pragma once

#include "llvmIRInclude.h"
#include "LLVMEmitter.h"
#include "FunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			class ModuleEmitter
			{
			public:
				ModuleEmitter(LLVMEmitter* pEmitter, std::unique_ptr<llvm::Module> pModule);
				
				FunctionEmitter AddMain();

				void declareFunction(const std::string& name, ValueType returnType)
				{
					_pEmitter->DeclareFunction(module(), name, returnType, nullptr);
				}
				void declareFunction(const std::string& name, ValueType returnType, ValueTypeList& args)
				{
					_pEmitter->DeclareFunction(module(), name, returnType, &args);
				}
				void declareFunction(const std::string& name, ValueType returnType, NamedValueTypeList& args)
				{
					_pEmitter->DeclareFunction(module(), name, returnType, args);
				}
				void declareFunction(const std::string& name, llvm::FunctionType* type)
				{
					_pEmitter->DeclareFunction(module(), name, type);
				}
				FunctionEmitter Function(const std::string& name, ValueType returnType, bool isPublic = false)
				{
					return Function(name, returnType, nullptr, isPublic);
				}
				FunctionEmitter Function(const std::string& name, ValueType returnType, ValueTypeList& args, bool isPublic = false)
				{
					return Function(name, returnType, &args, isPublic);
				}
				FunctionEmitter Function(const std::string& name, ValueType returnType, NamedValueTypeList& args, bool isPublic = false);
				FunctionEmitter Function(const std::string& name, ValueType returnType, std::initializer_list<ValueType> args, bool isPublic = false);

				//
				// Serialization
				//
				void writeBitcodeToFile(const std::string& filePath) const
				{
					writeToFile(filePath, true);
				}
				void writeAsmToFile(const std::string& filePath) const
				{
					writeToFile(filePath, false);
				}
				//
				// To emit declarations for standard C functions
				//
				void declarePrintf();
				void declareMalloc();
				void declareFree();
				//
				// Debugging
				//
				void dump()
				{
					_pModule->dump();
				}

			private:
				llvm::Module* module() const
				{
					return _pModule.get();
				}
				llvm::Function* getFunction(const std::string& name);
				FunctionEmitter Function(const std::string& name, ValueType returnType, ValueTypeList* pArgs, bool isPublic);
				void beginFunction(llvm::Function* pfn);
				void writeToFile(const std::string& filePath, bool isBitCode) const;
				llvm::Function::LinkageTypes linkage(bool isPublic);

			private:
				std::unique_ptr<llvm::Module> _pModule;
				LLVMEmitter* _pEmitter;
				// Reusable buffers
				ValueTypeList _valueTypeList;
			};
		}
	}
}