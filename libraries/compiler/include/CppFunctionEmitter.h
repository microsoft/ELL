#pragma once

#include "CppEmitter.h"
#include <functional>
#include "Variable.h"
#include "CppBlock.h"

namespace emll
{
	namespace compiler
	{
		///<summary>Class used to emit Cpp Functions by the CppCompiler</summary>
		class CppFunctionEmitter
		{
		public:
			CppFunctionEmitter();

			CppBlock* AppendBlock();
			CppBlock* CurrentBlock() const { return _pCurBlock; };
			CppBlock* MergeBlocks(CppBlock* pTarget, CppBlock* pSrc);

			CppFunctionEmitter& Comment(const std::string& comment);
			CppFunctionEmitter& Comment();
			CppFunctionEmitter& CommentText(const std::string& text);
			CppFunctionEmitter& EndComment();

			///<summary>Begin a new function with the given return type and args</summary>
			CppFunctionEmitter& Begin(const std::string& name, const ValueType returnType, const NamedValueTypeList& args);
			///<summary>End the current function</summary>
			CppFunctionEmitter& End();
			CppFunctionEmitter& EndStatement();

			///<summary>Emit a literal</summary>
			template<typename T>
			CppFunctionEmitter& Literal(T value);
			///<summary>Emit a named stack scalar </summary>
			CppFunctionEmitter& Var(ValueType type, const std::string& name);
			///<summary>Emit a named stack scalar with the given initial value</summary>
			template<typename T>
			CppFunctionEmitter& Var(const std::string& name, T data);

			///<summary>Emit a value</summary>
			CppFunctionEmitter& Value(const std::string& varName);
			///<summary>Emit a offset into a pointer</summary>
			CppFunctionEmitter& ValueAt(const std::string& name, int offset);
			///<summary>Emit a offset into a pointer</summary>
			CppFunctionEmitter& ValueAt(const std::string& name, const std::string& offsetVarName);
			
			CppFunctionEmitter& Value(Variable& var, int index);

			///<summary>Emit an Assign</summary>
			CppFunctionEmitter& Assign(const std::string& varName);
			template<typename T>
			///<summary>Emit an Assign</summary>
			CppFunctionEmitter& Assign(const std::string& varName, T value);
			///<summary>Emit an Assign</summary>
			CppFunctionEmitter& AssignValue(const std::string& varName, std::function<void()> value);

			///<summary>Emit an Assign</summary>
			CppFunctionEmitter& AssignValueAt(const std::string& destVarName, int offset);
			///<summary>Emit an Assign</summary>
			CppFunctionEmitter& AssignValueAt(const std::string& destVarName, const std::string& offsetVarName);
			///<summary>Emit an Assign</summary>
			CppFunctionEmitter& AssignValueAt(const std::string& destVarName, int offset, std::function<void()> value);

			///<summary>Emit an Assign and increment</summary>
			CppFunctionEmitter& IncrementUpdate(const std::string& varName);
			///<summary>Emit an Assign and increment</summary>
			CppFunctionEmitter& IncrementValueAt(const std::string& destVarName, int offset);
			///<summary>Emit an Assign and increment</summary>
			CppFunctionEmitter& IncrementValueAt(const std::string& destVarName, const std::string& offsetVarName);

			CppFunctionEmitter& AssignValue(Variable& var);
			CppFunctionEmitter& AssignValue(Variable& var, int offset);

			///<summary>Emit a math operator</summary>
			CppFunctionEmitter& Op(OperatorType op, std::function<void()> lValue, std::function<void()> rValue);
			///<summary>Emit a comparison</summary>
			CppFunctionEmitter& Cmp(ComparisonType cmp, std::function<void()> lValue, std::function<void()> rValue);

			///<summary>Begin a for loop</summary>
			CppFunctionEmitter& For(const std::string& iVarName, int count);
			///<summary>End a for loop</summary>
			CppFunctionEmitter& EndFor();
			
			///<summary>Begin a conditional block</summary>
			template<typename T>
			CppFunctionEmitter& If(const std::string&varName, ComparisonType cmp, T value);
			///<summary>Begin a conditional</summary>
			CppFunctionEmitter& If(std::function<void()> value);
			///<summary>Begin a conditional</summary>
			CppFunctionEmitter& Else();
			///<summary>End a conditional</summary>
			CppFunctionEmitter& EndIf();
			///<summary>Begin a conditional</summary>
			CppFunctionEmitter& IfInline(std::function<void()> value, std::function<void()> lVal, std::function<void()> rVal);

			///<summary>Return generated code</summary>
			std::string Code();

			CppFunctionEmitter& Clear();

		protected:
			CppFunctionEmitter& DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args);
			void MergeBlocks();

		private:
			CppBlockList _blocks;
			CppBlock* _pVariables;
			CppBlock* _pCurBlock = nullptr;
			//int _mergeCount = 0;
		};
	}
}

#include "../tcc/CppFunctionEmitter.tcc"