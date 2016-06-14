#pragma once

#include "llvmIRInclude.h"
#include "ValueType.h"

namespace emll
{
	namespace codegen
	{
		using ValueList = TypeList<llvm::Value*>;

		enum OperatorType
		{
			Add,
			Subtract,
			Multiply,
			DivideS,
			AddF,
			SubtractF,
			MultiplyF,
			DivideF,
		};

		class LLVMEmitter
		{
		public:
			LLVMEmitter();
			virtual ~LLVMEmitter();

			llvm::Type* valueType(ValueType type);

			llvm::Value* literal(int value);
			llvm::Value* literal(int64_t value);
			llvm::Value* literal(double value);
			llvm::Value* literal(const std::string& value);

			llvm::Value* addCast(llvm::Value* pValue, ValueType destType);
			llvm::Value* addGlobal(const std::string& name, const std::string& value);
			
			llvm::ReturnInst* addReturnVoid();
			llvm::ReturnInst* addReturn(llvm::Value* value);
			llvm::Value* addBinaryOp(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName = "");

			std::unique_ptr<llvm::Module> addModule(const std::string& name);
			
			llvm::Function* addDeclaration(llvm::Module* pModule, const std::string& name, ValueType returnType, ValueTypeList* pArgs);
			llvm::Function* addDeclaration(llvm::Module* pModule, const std::string& name, ValueType returnType, NamedValueTypeList& args);
			llvm::Function* addDeclaration(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type)
			{
				return static_cast<llvm::Function*>(pModule->getOrInsertFunction(name, type));
			}

			llvm::Function* addFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, ValueTypeList* pArgs);
			llvm::Function* addFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, NamedValueTypeList& args);

			llvm::BasicBlock* addBlock(llvm::Function* pfn, const std::string& label);
			llvm::BasicBlock* currentBlock()
			{
				return _pBuilder->GetInsertBlock();
			}
			void setCurrentBlock(llvm::BasicBlock* pBlock);

			llvm::CallInst* addCall(llvm::Function* pfn);
			llvm::CallInst* addCall(llvm::Function* pfn, llvm::Value* pArg);
			llvm::CallInst* addCall(llvm::Function* pfn, ValueList& args);
			
			llvm::PHINode* addPhi(ValueType type, llvm::Value* pLVal, llvm::BasicBlock* plBlock, llvm::Value* pRVal, llvm::BasicBlock* prBlock);

			llvm::Value* addArrayDeref(llvm::Value* pArray, llvm::Value* offset)
			{
				return _pBuilder->CreateGEP(pArray, offset);
			}

			llvm::LoadInst* addLoad(llvm::Value* pPtr)
			{
				return _pBuilder->CreateLoad(pPtr);
			}
			llvm::LoadInst* addLoad(llvm::Value* pPtr, llvm::Value* pValue)
			{
				return _pBuilder->CreateLoad(pPtr, pValue);
			}
			llvm::StoreInst* addStore(llvm::Value* pPtr, llvm::Value* pVal)
			{
				return _pBuilder->CreateStore(pVal, pPtr);
			}
			llvm::AllocaInst* addVariable(ValueType type);
			llvm::AllocaInst* addStackAlloc(ValueType type, int count);

			llvm::LLVMContext& context()
			{
				return _context;
			}

		private:
			llvm::Type* getValueType(ValueType type);
			void bindArgTypes(ValueTypeList& args);
			void bindArgTypes(NamedValueTypeList& args);
			void bindArgNames(llvm::Function* pfn, NamedValueTypeList& args);
			llvm::Function* createFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef);

		private:
			llvm::LLVMContext _context;
			std::unique_ptr<llvm::IRBuilder<>> _pBuilder;
			std::unordered_map<std::string, llvm::Value*> _stringLiterals;
			// Reusable buffers
			std::vector<llvm::Type *> _argTypes;
		};
	}
}