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
				// To emit declarations for standard C functions
				//
				void declarePrintf();
				void declareMalloc();
				void declareFree();
				//
				// Debugging
				//
				void Dump()
				{
					_pModule->dump();
				}

			private:
				llvm::Module* module() const
				{
					return _pModule.get();
				}
				llvm::Function* GetFunction(const std::string& name);
				FunctionEmitter Function(const std::string& name, ValueType returnType, ValueTypeList* pArgs, bool isPublic);
				void BeginFunction(llvm::Function* pfn);
				void WriteToFile(const std::string& filePath, bool isBitCode);
				void WriteToStream(std::ostream& os, bool isBitCode);
				llvm::Function::LinkageTypes Linkage(bool isPublic);

			private:
				std::unique_ptr<llvm::Module> _pModule;
				LLVMEmitter* _pEmitter;
				// Reusable buffers
				ValueTypeList _valueTypeList;
			};
		}
	}
}