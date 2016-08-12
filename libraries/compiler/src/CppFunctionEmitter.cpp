#include "CppFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		CppFunctionEmitter::CppFunctionEmitter()
		{
			_variables.IncreaseIndent();
			_code.IncreaseIndent();
		}

		CppFunctionEmitter& CppFunctionEmitter::Clear()
		{
			_root.Clear();
			_variables.Clear();
			_code.Clear();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Begin(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
		{
			Clear();
			_root.DeclareFunction(name, returnType, args).NewLine()
				.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::End()
		{
			_root.Append(_variables);
			_root.NewLine();
			_root.Append(_code);
			_root.EndBlock();

			_variables.Clear();
			_code.Clear();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndStatement()
		{
			_code.EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Var(ValueType type, const std::string& name)
		{
			_variables.Var(type, name).EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Value(const std::string& varName)
		{
			_code.Identifier(varName);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::ValueAt(const std::string& name, int offset)
		{
			_code.Identifier(name)
				.Offset(offset);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::ValueAt(const std::string& name, const std::string& offsetVarName)
		{
			_code.Identifier(name)
				.Offset(offsetVarName);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Value(Variable& var, int index)
		{
			if (var.IsScalar())
			{
				if (index > 0)
				{
					throw new CompilerException(CompilerError::vectorVariableExpected);
				}
				if (!var.IsLiteral())
				{
					Value(var.EmittedName());
				}
			}
			else
			{
				if (index >= var.Dimension())
				{
					throw new CompilerException(CompilerError::indexOutOfRange);
				}
				ValueAt(var.EmittedName(), index);
			}
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Assign(const std::string& varName)
		{
			_code.Assign(varName).Space();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValue(const std::string& varName, std::function<void()> value)
		{
			_code.Assign(varName).Space();
			value();
			EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValueAt(const std::string& destVarName, int offset)
		{
			_code.AssignValueAt(destVarName, offset).Space();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValueAt(const std::string& destVarName, const std::string& offsetVarName)
		{
			_code.AssignValueAt(destVarName, offsetVarName).Space();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValueAt(const std::string& destVarName, int offset, std::function<void()> value)
		{
			_code.AssignValueAt(destVarName, offset).Space();
			value();
			EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValue(Variable& var)
		{
			if (var.IsScalar())
			{
				Assign(var.EmittedName());
			}
			else
			{
				AssignValueAt(var.EmittedName(), 0);
			}
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValue(Variable& var, int offset)
		{
			if (var.IsScalar())
			{
				if (offset > 0)
				{
					throw new CompilerException(CompilerError::indexOutOfRange);
				}
				Assign(var.EmittedName());
			}
			else
			{
				if (offset >= var.Dimension())
				{
					throw new CompilerException(CompilerError::indexOutOfRange);
				}
				AssignValueAt(var.EmittedName(), offset);
			}
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Op(OperatorType op, std::function<void()> lValue, std::function<void()> rValue)
		{
			lValue();
			_code.Space();
			_code.Operator(op).Space();
			rValue();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Cmp(ComparisonType cmp, std::function<void()> lValue, std::function<void()> rValue)
		{
			_code.OpenParan();
			lValue();
			_code.Space();
			_code.Cmp(cmp).Space();
			rValue();
			_code.CloseParan();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::BeginFor(const std::string& iVarName, int count)
		{
			_code.For()
				.OpenParan()
				.Var<int>(iVarName).Space()
				.Assign().Space()
				.Literal(0).Semicolon().Space()
				.Cmp<int>(iVarName, ComparisonType::Lt, count).Semicolon().Space()
				.Identifier(iVarName).Increment()
				.CloseParan().NewLine()
				.BeginBlock();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndFor()
		{
			_code.EndBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::BeginIf(std::function<void()> value)
		{
			_code.If().OpenParan();
			value();
			_code.CloseParan().NewLine()
					.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::BeginElse()
		{
			_code.Else().NewLine()
				.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndIf()
		{
			_code.EndBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::IfInline(std::function<void()> value, std::function<void()> lVal, std::function<void()> rVal)
		{
			_code.OpenParan();
			{
				value();
			}
			_code.CloseParan().Space();
			_code.Question().Space();
			{
				lVal();
				_code.Space();
			}
			_code.Colon().Space();
			{
				rVal();
			}
			EndStatement();
			return *this;
		}
	}
}