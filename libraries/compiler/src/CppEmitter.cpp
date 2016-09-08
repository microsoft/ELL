////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppEmitter.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CppEmitter.h"
#include "CompilerException.h"
#include <cassert>
#include <iostream>
#include <sstream>

namespace emll
{
	namespace compiler
	{
		static const std::string c_Const = "const";
		static const std::string c_Static = "static";
		static const std::string c_VoidType = "void";
		static const std::string c_ByteType = "uint8_t";
		static const std::string c_IntType = "int";
		static const std::string c_DoubleType = "double";
		static const std::string c_ForKeyword = "for";
		static const std::string c_IfKeyword = "if";
		static const std::string c_ElseKeyword = "else";

		CppEmitter::CppEmitter()	
		{
		}
		
		CppEmitter& CppEmitter::Comment(const std::string& text)
		{
			OpenComment().Space();
			{
				AppendRaw(text).Space();
			}
			CloseComment().NewLine();
			return *this;
		}

		CppEmitter& CppEmitter::Space()
		{
			_writer.Write(" ");
			return *this;
		}

		CppEmitter& CppEmitter::NewLine()
		{
			_writer.WriteNewLine();
			return *this;
		}

		CppEmitter& CppEmitter::Semicolon()
		{
			_writer.Write(';');
			return *this;
		}

		CppEmitter& CppEmitter::Comma()
		{
			_writer.Write(',');
			return *this;
		}

		CppEmitter& CppEmitter::OpenComment()
		{
			_writer.Write("/*");
			return *this;
		}

		CppEmitter& CppEmitter::CloseComment()
		{
			_writer.Write("*/");
			return *this;
		}

		CppEmitter& CppEmitter::OpenBrace()
		{
			_writer.Write('{');
			return *this;
		}
		
		CppEmitter& CppEmitter::CloseBrace()
		{
			_writer.Write('}');
			return *this;
		}

		CppEmitter& CppEmitter::OpenParan()
		{
			_writer.Write('(');
			return *this;
		}

		CppEmitter& CppEmitter::CloseParan()
		{
			_writer.Write(')');
			return *this;
		}

		CppEmitter& CppEmitter::OpenBracket()
		{
			_writer.Write('[');
			return *this;
		}

		CppEmitter& CppEmitter::CloseBracket()
		{
			_writer.Write(']');
			return *this;
		}

		CppEmitter& CppEmitter::Quote()
		{
			_writer.Write('"');
			return *this;
		}
		
		CppEmitter& CppEmitter::Question()
		{
			_writer.Write('?');
			return *this;
		}

		CppEmitter& CppEmitter::Colon()
		{
			_writer.Write(':');
			return *this;
		}

		CppEmitter& CppEmitter::Assign()
		{
			_writer.Write('=');
			return *this;
		}

		CppEmitter& CppEmitter::Asterisk()
		{
			_writer.Write('*');
			return *this;
		}

		CppEmitter& CppEmitter::Increment()
		{
			_writer.Write("++");
			return *this;
		}

		CppEmitter& CppEmitter::IncrementUpdate()
		{
			_writer.Write("+=");
			return *this;
		}

		CppEmitter& CppEmitter::EndStatement()
		{
			return Semicolon().NewLine();
		}

		CppEmitter& CppEmitter::Operator(OperatorType op)
		{
			switch (op)
			{
				case OperatorType::Add:
				case OperatorType::AddF:
					_writer.Write('+');
					break;
				case OperatorType::Subtract:
				case OperatorType::SubtractF:
					_writer.Write('-');
					break;
				case OperatorType::Multiply:
				case OperatorType::MultiplyF:
					_writer.Write('*');
					break;
				case emll::compiler::OperatorType::DivideS:
				case emll::compiler::OperatorType::DivideF:
					_writer.Write('/');
					break;
				default:
					throw new CompilerException(CompilerError::operatorTypeNotSupported);
			}
			return *this;
		}

		CppEmitter& CppEmitter::Cmp(ComparisonType cmp)
		{
			switch (cmp)
			{
				case ComparisonType::Eq:
				case ComparisonType::EqF:
					_writer.Write("==");
					break;
				case ComparisonType::Lt:
				case ComparisonType::LtF:
					_writer.Write('<');
					break;
				case ComparisonType::Lte:
				case ComparisonType::LteF:
					_writer.Write("<=");
					break;
				case ComparisonType::Gt:
				case ComparisonType::GtF:
					_writer.Write('>');
					break;
				case ComparisonType::Gte:
				case ComparisonType::GteF:
					_writer.Write(">=");
					break;
				case ComparisonType::Neq:
				case ComparisonType::NeqF:
					_writer.Write("!=");
					break;
				default:
					throw new CompilerException(CompilerError::comparisonTypeNotSupported);
			}
			return *this;
		}

		CppEmitter& CppEmitter::Const()
		{
			return Token(c_Const);
		}

		CppEmitter& CppEmitter::Static()
		{
			return Token(c_Static);
		}

		CppEmitter& CppEmitter::Offset(int offset)
		{
			return OpenBracket()
				.Literal(offset)
				.CloseBracket();
		}

		CppEmitter& CppEmitter::Offset(const std::string& offsetVarName)
		{
			return OpenBracket()
				.Identifier(offsetVarName)
				.CloseBracket();
		}

		CppEmitter& CppEmitter::Dimension(int size)
		{
			OpenBracket();
			if (size > 0)
			{
				Literal(size);
			}
			return CloseBracket();
		}

		CppEmitter& CppEmitter::Token(const std::string& token)
		{
			_writer.Write(token);
			return *this;
		}

		CppEmitter& CppEmitter::Type(const ValueType type)
		{
			switch (type)
			{
				case ValueType::Void:
					Token(c_VoidType);
					break;
				case ValueType::Byte:
					Token(c_ByteType); 
					break;
				case ValueType::Int32:
					Token(c_IntType);
					break;
				case ValueType::Double:
					Token(c_DoubleType);
					break;
				case ValueType::PVoid:
					Token(c_VoidType).Asterisk();
					break;
				case ValueType::PByte:
					Token(c_ByteType).Asterisk();
					break;
				case ValueType::PInt32:
					Token(c_IntType).Asterisk();
					break;
				case ValueType::PDouble:
					Token(c_DoubleType).Asterisk();
					break;
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
			return *this;
		}

		CppEmitter& CppEmitter::Literal(const std::string& value)
		{
			Quote();
			_writer.Write(value);
			Quote();
			return *this;
		}

		CppEmitter& CppEmitter::Var(const ValueType type, const std::string& name)
		{
			return Type(type)
				.Space()
				.Identifier(name);
		}

		CppEmitter& CppEmitter::Var(const NamedValueType& var)
		{
			return Var(var.second, var.first);
		}

		CppEmitter& CppEmitter::Vars(const NamedValueTypeList& args)
		{
			for(size_t i = 0; i < args.size(); ++i)
			{
				if (i > 0)
				{
					Comma().Space();
				}
				Var(args[i]);
			}
			return *this;
		}

		CppEmitter& CppEmitter::DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
		{
			return Type(returnType).Space()
					.Token(name)
					.OpenParan()
					.Vars(args)
					.CloseParan();
		}

		CppEmitter& CppEmitter::For()
		{
			return Token(c_ForKeyword);
		}

		CppEmitter& CppEmitter::If()
		{
			return Token(c_IfKeyword);
		}

		CppEmitter& CppEmitter::Else()
		{
			return Token(c_ElseKeyword);
		}

		CppEmitter& CppEmitter::Assign(const std::string& varName)
		{
			return Identifier(varName).Space()
					.Assign();
		}

		CppEmitter& CppEmitter::IncrementUpdate(const std::string& varName)
		{
			return Identifier(varName).Space()
				.IncrementUpdate();
		}

		CppEmitter& CppEmitter::AssignValueAt(const std::string& varName, const int offset)
		{
			return Identifier(varName)
				.Offset(offset).Space()
				.Assign();
		}

		CppEmitter& CppEmitter::AssignValueAt(const std::string& varName, const std::string& offsetVarName)
		{
			return Identifier(varName)
				.Offset(offsetVarName).Space()
				.Assign();
		}

		CppEmitter& CppEmitter::IncrementValueAt(const std::string& varName, const int offset)
		{
			return Identifier(varName)
				.Offset(offset).Space()
				.IncrementUpdate();
		}

		CppEmitter& CppEmitter::IncrementValueAt(const std::string& varName, const std::string& offsetVarName)
		{
			return Identifier(varName)
				.Offset(offsetVarName).Space()
				.IncrementUpdate();
		}

		CppEmitter& CppEmitter::Clear()
		{
			_writer.Clear();
			return *this;
		}

		CppEmitter& CppEmitter::BeginBlock()
		{
			return OpenBrace().NewLine()
				  .IncreaseIndent();
		}

		CppEmitter& CppEmitter::EndBlock()
		{
			return DecreaseIndent()
				.CloseBrace().NewLine();
		}

		CppEmitter& CppEmitter::AppendRaw(const std::string& code)
		{
			_writer.WriteRaw(code);
			return *this;
		}

		CppEmitter& CppEmitter::Append(CppEmitter& emitter)
		{
			std::string code = emitter.Code();
			if (code.length() == 0)
			{
				return *this;
			}
			int indentDelta = Indent() - emitter.Indent();
			if (indentDelta <= 0)
			{ 
				return AppendRaw(code);
			}
			else
			{
				return AppendReindent(code, indentDelta);
			}
		}

		CppEmitter& CppEmitter::Append(CppEmitter* pEmitter)
		{
			assert(pEmitter != nullptr);
			Append(*pEmitter);
			return *this;
		}

		CppEmitter& CppEmitter::IncreaseIndent()
		{
			_writer.IncreaseIndent();
			return *this;
		}

		CppEmitter& CppEmitter::DecreaseIndent()
		{
			_writer.DecreaseIndent();
			return *this;
		}

		CppEmitter& CppEmitter::AppendReindent(std::string& code, int indentDelta)
		{
			std::istringstream lines(code);
			std::string line;
			while (std::getline(lines, line))
			{
				_writer.WriteRaw(line, indentDelta).WriteNewLine();
			}
			return *this;
		}
	}
}