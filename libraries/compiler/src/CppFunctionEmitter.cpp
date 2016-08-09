#include "CppFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		CppFunctionEmitter::CppFunctionEmitter()
		{
		}

		CppFunctionEmitter& CppFunctionEmitter::Begin(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
		{
			_emitter.Clear()
				.DeclareFunction(name, returnType, args)
				.NewLine()
				.OpenBrace()
				.NewLine()
				.IncreaseIndent();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::End()
		{
			_emitter.DecreaseIndent()
					.CloseBrace()
					.NewLine();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Var(ValueType type, const std::string& name)
		{
			_emitter.Var(type, name)
					.Semicolon()
					.NewLine();
			return *this;
		}
	}
}