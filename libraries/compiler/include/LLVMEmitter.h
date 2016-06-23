#pragma once

#include "llvmIRInclude.h"
#include "ValueType.h"
#include <unordered_map>

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			using ValueList = TypeList<llvm::Value*>;

			enum class OperatorType
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

			enum class ComparisonType
			{
				Eq,
				Lt,
				Lte,
				Gt,
				Gte,
				Neq
			};

			class VariableTable
			{
			public:
				llvm::Value* Get(const std::string& name);
				void Set(const std::string& name, llvm::Value* pVal);

			private:
				std::unordered_map<std::string, llvm::Value*> _map;
			};


			class LLVMEmitter
			{
			public:
				LLVMEmitter();
				virtual ~LLVMEmitter();

				llvm::Type* Type(ValueType type);

				llvm::Value* Literal(int value);
				llvm::Value* Literal(int64_t value);
				llvm::Value* Literal(double value);
				llvm::Value* Literal(const std::string& value);

				llvm::Value* Cast(llvm::Value* pValue, ValueType destType);
				llvm::Value* Global(const std::string& name, const std::string& value);

				llvm::ReturnInst* ReturnVoid();
				llvm::ReturnInst* Return(llvm::Value* value);
				llvm::Value* BinaryOp(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName = "");

				std::unique_ptr<llvm::Module> AddModule(const std::string& name);

				llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, ValueTypeList* pArgs);
				llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, NamedValueTypeList& args);
				llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type)
				{
					return static_cast<llvm::Function*>(pModule->getOrInsertFunction(name, type));
				}

				llvm::Function* Function(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, ValueTypeList* pArgs);
				llvm::Function* Function(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, NamedValueTypeList& args);

				llvm::BasicBlock* Block(llvm::Function* pfn, const std::string& label);
				llvm::BasicBlock* BlockAfter(llvm::Function* pfn, llvm::BasicBlock* pPrevBlock, const std::string& label);

				// This block is not part of any function yet. You'll need to insert it manually
				llvm::BasicBlock* Block(const std::string& label);
				llvm::BasicBlock* CurrentBlock()
				{
					return _pBuilder->GetInsertBlock();
				}
				void SetCurrentBlock(llvm::BasicBlock* pBlock);

				llvm::CallInst* Call(llvm::Function* pfn);
				llvm::CallInst* Call(llvm::Function* pfn, llvm::Value* pArg);
				llvm::CallInst* Call(llvm::Function* pfn, ValueList& args);

				llvm::PHINode* Phi(ValueType type, llvm::Value* pLVal, llvm::BasicBlock* plBlock, llvm::Value* pRVal, llvm::BasicBlock* prBlock);

				llvm::Value* ArrayDeref(llvm::Value* pArray, llvm::Value* offset)
				{
					return _pBuilder->CreateGEP(pArray, offset);
				}

				llvm::LoadInst* Load(llvm::Value* pPtr)
				{
					return _pBuilder->CreateLoad(pPtr);
				}
				llvm::LoadInst* Load(llvm::Value* pPtr, llvm::Value* pValue)
				{
					return _pBuilder->CreateLoad(pPtr, pValue);
				}
				llvm::StoreInst* Store(llvm::Value* pPtr, llvm::Value* pVal)
				{
					return _pBuilder->CreateStore(pVal, pPtr);
				}

				llvm::AllocaInst* Variable(ValueType type);
				llvm::AllocaInst* Variable(ValueType type, const std::string& name);
				llvm::AllocaInst* Variable(llvm::Type* pType, const std::string& name);

				llvm::AllocaInst* StackAlloc(ValueType type, int count);


				llvm::Value* Cmp(ComparisonType type, llvm::Value* pLVal, llvm::Value* pRVal);

				llvm::BranchInst* Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);
				llvm::BranchInst* Branch(llvm::BasicBlock* pDest);

				llvm::LLVMContext& Context()
				{
					return _context;
				}

			private:
				llvm::Type* GetValueType(ValueType type);
				void BindArgTypes(ValueTypeList& args);
				void BindArgTypes(NamedValueTypeList& args);
				void BindArgNames(llvm::Function* pfn, NamedValueTypeList& args);
				llvm::Function* CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef);

			private:
				llvm::LLVMContext _context;
				std::unique_ptr<llvm::IRBuilder<>> _pBuilder;
				VariableTable _stringLiterals;
				// Reusable buffers
				std::vector<llvm::Type *> _argTypes;
			};
		}
	}
}