////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IREmitter.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "LLVMInclude.h"
#include "Types.h"
#include <unordered_map>
#include "Exception.h"
#include "SymbolTable.h"

namespace emll
{
	namespace compiler
	{
		///<summary>A list of LLVM IR Value*</summary>
		using IRValueList = TypeList<llvm::Value*>;
		///<summary>Symbol Table that maps Symbol Names to emitted IR Value*</summary>
		using IRVariableTable = SymbolTable<llvm::Value*, nullptr>;
		///<summary>Convert LLVM errors into an EMLL styled exception</summary>
		using LLVMException = utilities::ErrorCodeException<std::error_code>;

		///<summary>Wraps the LLVM API with an easy to use object model that hides some unncessary detail. 
		/// Incorporates our own x-compiler abstractions such as ValueType and OperatorType. 
		/// </summary>
		class IREmitter
		{
		public:
			///<summary>Construct a new emitter</summary>
			IREmitter();

			///<summary>Get the LLVM Type Information for a ValueType</summary>
			llvm::Type* Type(const ValueType type);
			///<summary>Get the LLVM Type Information for an ARRAY of ValueType, with the given size</summary>
			llvm::ArrayType* ArrayType(const ValueType type, uint64_t size);
			///<summary>Get the LLVM Type Information for a vector of ValueType, with the given size</summary>
			llvm::VectorType* VectorType(const ValueType type, uint64_t size);

			///<summary>Emit a boolean literal</summary>
			llvm::Constant* Literal(const bool value);
			///<summary>Emit a byte literal</summary>
			llvm::Constant* Literal(const uint8_t value);
			///<summary>Emit a short literal</summary>
			llvm::Constant* Literal(const short value);
			///<summary>Emit an Int32 literal</summary>
			llvm::Constant* Literal(const int value);
			///<summary>Emit a Int64 literal</summary>
			llvm::Constant* Literal(const int64_t value);
			///<summary>Emit a double literal</summary>
			llvm::Constant* Literal(const double value);
			///<summary>Emit a string literal</summary>
			llvm::Value* Literal(const char* pValue);
			///<summary>Emit a string literal</summary>
			llvm::Value* Literal(const std::string& value);
			///<summary>Emit a named string literal</summary>
			llvm::Value* Literal(const std::string& name, const std::string& value);
			///<summary>Emit a literal array of bytes</summary>
			llvm::Constant* Literal(const std::vector<uint8_t>& value);
			///<summary>Emit a literal array of doubles</summary>
			llvm::Constant* Literal(const std::vector<double>& value);
			///<summary>Emit a literal array of Int32</summary>
			llvm::Constant* Literal(const std::vector<int>& value);
			///<summary>Emit a literal array of Int64</summary>
			llvm::Constant* Literal(const std::vector<int64_t>& value);
			///<summary>Emit a Zero value of the given type</summary>
			llvm::Constant* Zero(const ValueType type);

			///<summary>Cast the value to the given type</summary>
			llvm::Value* Cast(llvm::Value* pValue, const ValueType destType);
			///<summary>Cast the value to a float</summary>
			llvm::Value* CastFloat(llvm::Value* pValue, const ValueType destType);
			///<summary>Cast the value to an int</summary>
			llvm::Value* CastInt(llvm::Value* pValue, const ValueType destType);

			///<summary>Emit a return VOID</summary>
			llvm::ReturnInst* ReturnVoid();
			///<summary>Emit a return the given data</summary>
			llvm::ReturnInst* Return(llvm::Value* pValue);

			///<summary>Emit a binary operation, with an optional name for the result</summary>
			llvm::Value* BinaryOp(const OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName = "");
			///<summary>Emit a binary comparison</summary>
			llvm::Value* Cmp(const ComparisonType type, llvm::Value* pLVal, llvm::Value* pRVal);

			///<summary>Emit a declaration for an extern function</summary>
			llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, const ValueType returnType, const ValueTypeList* pArgs = nullptr);
			///<summary>Emit a declaration for an extern function</summary>
			llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, const ValueType returnType, const NamedValueTypeList& args);
			///<summary>Emit a declaration for an extern function</summary>
			llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type);

			///<summary>Begin a new function - emits the function declaration and args</summary>
			llvm::Function* Function(llvm::Module* pModule, const std::string& name, const ValueType returnType, llvm::Function::LinkageTypes linkage, const ValueTypeList* pArgs);
			///<summary>Begin a new function - emits the function declaration and args</summary>
			llvm::Function* Function(llvm::Module* pModule, const std::string& name, const ValueType returnType, llvm::Function::LinkageTypes linkage, const NamedValueTypeList& args);

			///<summary>Begin a new code block in the given function</summary>
			llvm::BasicBlock* Block(llvm::Function* pfn, const std::string& label);
			///<summary>Begin a new labeled code block in the given function. Add the block AFTER the given one. Commonly used for loops and branch statements</summary>
			llvm::BasicBlock* BlockAfter(llvm::Function* pfn, llvm::BasicBlock* pPrevBlock, const std::string& label);

			///<summary>New labeled block but not part of a function yet. You'll need to insert it into a function as needed</summary>
			llvm::BasicBlock* Block(const std::string& label);
			///<summary>The code block that all emitted instructions are being written to</summary>
			llvm::BasicBlock* CurrentBlock()
			{
				return _builder.GetInsertBlock();
			}
			///<summary>Change the current target block</summary>
			void SetCurrentBlock(llvm::BasicBlock* pBlock);

			///<summary>Call the given function</summary>
			llvm::CallInst* Call(llvm::Function* pfn);
			///<summary>Call the given function with one argument</summary>
			llvm::CallInst* Call(llvm::Function* pfn, llvm::Value* pArg);
			///<summary>Call the given function with a multiple arguments</summary>
			llvm::CallInst* Call(llvm::Function* pfn, const IRValueList& args);
			///<summary>Locates an intrinsic function with a signature matching the description in args</summary>
			llvm::Function* GetIntrinsic(llvm::Module* pModule, llvm::Intrinsic::ID id, const ValueTypeList& args);
			///<summary>Emit a Phi instruction</summary>
			llvm::PHINode* Phi(const ValueType type, llvm::Value* pLVal, llvm::BasicBlock* plBlock, llvm::Value* pRVal, llvm::BasicBlock* prBlock);

			///<summary>Emit a pointer to an item in an array</summary>
			llvm::Value* PtrOffset(llvm::Value* pArray, llvm::Value* pOffset, const std::string& name = "");
			///<summary>Get a pointer to the global array</summary>
			llvm::Value* Ptr(llvm::GlobalVariable* pArray);
			///<summary>Emit a pointer to an item in a Global array of primitive types</summary>
			llvm::Value* PtrOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset);
			///<summary>Emit a pointer to a FIELD in a STRUCT in an in a Global array of Structs</summary>
			llvm::Value* PtrOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset, llvm::Value* pFieldOffset);

			///<summary>Load the value referenced by a pointer into a register</summary>
			llvm::LoadInst* Load(llvm::Value* pPtr);
			///<summary>Load the value referenced by a pointer into a named register</summary>
			llvm::LoadInst* Load(llvm::Value* pPtr, const std::string& name);
			///<summary>Store a value into the pointer location</summary>
			llvm::StoreInst* Store(llvm::Value* pPtr, llvm::Value* pVal);

			///<summary>Create primitive local variable</summary>
			llvm::AllocaInst* Variable(const ValueType type);
			///<summary>Create named primitive local variable</summary>
			llvm::AllocaInst* Variable(const ValueType type, const std::string& name);
			///<summary>Create a named local variable of arbitrary type</summary>
			llvm::AllocaInst* Variable(llvm::Type* pType, const std::string& name);
			///<summary>Stack alloc an array of primitive types</summary>
			llvm::AllocaInst* StackAlloc(const ValueType type, int count);

			///<summary>Emit a conditional branch</summary>
			llvm::BranchInst* Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);
			///<summary>Emit an UN-conditional branch - i.e. always jump to the destination block</summary>
			llvm::BranchInst* Branch(llvm::BasicBlock* pDest);

			///<summary>Declare a new Struct with the given members</summary>
			llvm::StructType* Struct(const std::string& name, ValueTypeList& members);

			///<summary>Add a new module with the given name</summary>
			std::unique_ptr<llvm::Module> AddModule(const std::string& name);

			///<summary>Move memory</summary>
			llvm::CallInst* MemMove(llvm::Value* pSrc, llvm::Value* pDest, llvm::Value* pCountBytes);
			///<summary>Copy memory</summary>
			llvm::CallInst* MemCopy(llvm::Value* pSrc, llvm::Value* pDest, llvm::Value* pCountBytes);

			///<summary>Direct access to the LLVMContext.</summary>
			llvm::LLVMContext& Context()
			{
				return _context;
			}

		private:

			llvm::Type* GetValueType(const ValueType type);
			int SizeOf(const ValueType type);
			llvm::Constant* Integer(const ValueType type, const uint64_t value);
			void BindTypes(const ValueTypeList& args);
			void BindArgTypes(const NamedValueTypeList& args);
			void BindArgNames(llvm::Function* pfn, const NamedValueTypeList& args);
			llvm::Function* CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef);
			llvm::Value* Zero();

		private:
			llvm::LLVMContext _context;				// LLVM Emission Context
			llvm::IRBuilder<> _builder;				// IRBuilder API
			IRVariableTable _stringLiterals;			// String literals are emitted as constants. We have to track them ourselves to prevent dupes
			llvm::Value* _pZeroLiteral = nullptr;
			// Reusable buffers
			std::vector<llvm::Type *> _types;		
		};
	}

}
