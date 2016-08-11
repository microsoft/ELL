#pragma once

#include "CppEmitter.h"
#include <functional>

namespace emll
{
	namespace compiler
	{
		///<summary>Class used to emit Cpp Functions by the CppCompiler</summary>
		class CppFunctionEmitter
		{
		public:
			CppFunctionEmitter();

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

			///<summary>Emit a offset into a pointer</summary>
			CppFunctionEmitter& Value(const std::string& varName);
			CppFunctionEmitter& ValueAt(const std::string& name, int offset);
			CppFunctionEmitter& ValueAt(const std::string& name, const std::string& offsetVarName);

			CppFunctionEmitter& AssignValue(const std::string& varName);
			CppFunctionEmitter& AssignValue(const std::string& varName, std::function<void(CppFunctionEmitter& fn)> value);
			CppFunctionEmitter& AssignValueAt(const std::string& destVarName, int offset);
			CppFunctionEmitter& AssignValueAt(const std::string& destVarName, const std::string& offsetVarName);
			CppFunctionEmitter& AssignValueAt(const std::string& destVarName, int offset, std::function<void(CppFunctionEmitter& fn)> value);

			CppFunctionEmitter& Op(OperatorType op, std::function<void(CppFunctionEmitter& fn)> lValue, std::function<void(CppFunctionEmitter& fn)> rValue);

			CppFunctionEmitter& BeginFor(const std::string& iVarName, int count);
			CppFunctionEmitter& EndFor();
			
			template<typename T>
			CppFunctionEmitter& BeginIf(const std::string&varName, ComparisonType cmp, T value);
			CppFunctionEmitter& BeginElse();
			CppFunctionEmitter& EndIf();

			std::string Code() { return _emitter.Code(); }

		private:
			CppEmitter _emitter;
		};
	}
}

#include "../tcc/CppFunctionEmitter.tcc"