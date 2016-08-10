#pragma once

#include "CppEmitter.h"

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

			///<summary>Emit a literal</summary>
			template<typename T>
			CppFunctionEmitter& Literal(T value);

			///<summary>Emit a named stack scalar </summary>
			CppFunctionEmitter& Var(ValueType type, const std::string& name);
			///<summary>Emit a named stack scalar with the given initial value</summary>
			template<typename T>
			CppFunctionEmitter& Var(const std::string& name, T data);

			///<summary>Emit a offset into a pointer</summary>
			CppFunctionEmitter& PtrOffset(const std::string& name, int offset);

			std::string Code() { return _emitter.Code(); }

		private:
			CppEmitter _emitter;
		};
	}
}

#include "../tcc/CppFunctionEmitter.tcc"