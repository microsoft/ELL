#include "CppFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		CppFunctionEmitter::CppFunctionEmitter()
		{
		}

		CppBlock* CppFunctionEmitter::AppendBlock()
		{
			auto pNewBlock = _blocks.AddBlock();
			if (_pCurBlock != nullptr)
			{
				pNewBlock->Indent() = _pCurBlock->Indent();
			}
			_pCurBlock = pNewBlock;
			return pNewBlock;
		}

		CppBlock* CppFunctionEmitter::MergeBlocks(CppBlock* pTarget, CppBlock* pSrc)
		{
			pTarget->Append(pSrc);
			if (pSrc == _pCurBlock)
			{
				_pCurBlock = pTarget;
			}
			_blocks.Remove(pSrc);
			return pTarget;
		}

		CppFunctionEmitter& CppFunctionEmitter::Clear()
		{
			_blocks.Clear();
			_pCurBlock = nullptr;
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Comment(const std::string& comment)
		{
			_pCurBlock->Comment(comment);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Comment()
		{
			_pCurBlock->OpenComment();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::CommentText(const std::string& text)
		{
			_pCurBlock->AppendRaw(text);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndComment()
		{
			_pCurBlock->CloseComment().NewLine();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Begin(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
		{
			Clear();
			DeclareFunction(name, returnType, args);
			_pVariables = AppendBlock();
			_pCurBlock = AppendBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::End()
		{
			_pCurBlock->EndBlock();
			_pVariables->NewLine();
			MergeBlocks();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndStatement()
		{
			_pCurBlock->EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Var(ValueType type, const std::string& name)
		{
			_pVariables->Var(type, name).EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Value(const std::string& varName)
		{
			_pCurBlock->Identifier(varName);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::ValueAt(const std::string& name, int offset)
		{
			_pCurBlock->Identifier(name)
				.Offset(offset);
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::ValueAt(const std::string& name, const std::string& offsetVarName)
		{
			_pCurBlock->Identifier(name)
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
			_pCurBlock->Assign(varName).Space();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValue(const std::string& varName, std::function<void()> value)
		{
			_pCurBlock->Assign(varName).Space();
			value();
			EndStatement();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValueAt(const std::string& destVarName, int offset)
		{
			_pCurBlock->AssignValueAt(destVarName, offset).Space();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValueAt(const std::string& destVarName, const std::string& offsetVarName)
		{
			_pCurBlock->AssignValueAt(destVarName, offsetVarName).Space();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::AssignValueAt(const std::string& destVarName, int offset, std::function<void()> value)
		{
			_pCurBlock->AssignValueAt(destVarName, offset).Space();
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
			_pCurBlock->Space();
			_pCurBlock->Operator(op).Space();
			rValue();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Cmp(ComparisonType cmp, std::function<void()> lValue, std::function<void()> rValue)
		{
			_pCurBlock->OpenParan();
			{
				lValue();
				_pCurBlock->Space().Cmp(cmp).Space();
				rValue();
			}
			_pCurBlock->CloseParan();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::For(const std::string& iVarName, int count)
		{
			_pCurBlock->For();
			_pCurBlock->OpenParan();
			{
				_pCurBlock->Var<int>(iVarName).Space()
					.Assign().Space()
					.Literal(0).Semicolon().Space()
					.Cmp<int>(iVarName, ComparisonType::Lt, count).Semicolon().Space()
					.Identifier(iVarName).Increment();
			}
			_pCurBlock->CloseParan().NewLine();
			_pCurBlock->BeginBlock();

			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndFor()
		{
			_pCurBlock->EndBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::If(std::function<void()> value)
		{
			_pCurBlock->If().OpenParan();
			value();
			_pCurBlock->CloseParan().NewLine()
					.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::Else()
		{
			_pCurBlock->Else().NewLine()
				.BeginBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::EndIf()
		{
			_pCurBlock->EndBlock();
			return *this;
		}

		CppFunctionEmitter& CppFunctionEmitter::IfInline(std::function<void()> value, std::function<void()> lVal, std::function<void()> rVal)
		{
			_pCurBlock->OpenParan();
			{
				value();
			}
			_pCurBlock->CloseParan().Space();
			_pCurBlock->Question().Space();
			{
				lVal();
				_pCurBlock->Space();
			}
			_pCurBlock->Colon().Space();
			{
				rVal();
			}
			EndStatement();
			return *this;
		}

		std::string CppFunctionEmitter::Code()
		{
			return _pCurBlock->Code();
		}

		CppFunctionEmitter& CppFunctionEmitter::DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
		{
			CppBlock* pDeclaration = AppendBlock();
			pDeclaration->DeclareFunction(name, returnType, args).NewLine()
						  .BeginBlock();
			return *this;
		}

		void CppFunctionEmitter::MergeBlocks()
		{
			_blocks.Merge();
			_pCurBlock = _blocks.First();
		}
	}
}