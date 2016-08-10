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
				.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::End()
		{
			_emitter.EndBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Var(ValueType type, const std::string& name)
		{
			_emitter.Var(type, name).Semicolon()
					.NewLine();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::PtrOffset(const std::string& name, int offset)
		{
			_emitter.Identifier(name)
					.Offset(offset);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::BeginFor(const std::string& iVarName, int count)
		{
			_emitter.For()
				.OpenParan()
				.Identifier(iVarName).SetVar<int>(0).Semicolon().Space()
				.Cmp<int>(iVarName, ComparisonType::Lt, count).Semicolon().Space()
				.Identifier(iVarName).Increment()
				.CloseParan().NewLine()
				.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndFor()
		{
			_emitter.EndBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::BeginElse()
		{
			_emitter.Else().NewLine()
				.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndIf()
		{
			_emitter.EndBlock();
			return *this;
		}

	}
}