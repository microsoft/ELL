////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppEmitter.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CppEmitter.h"
#include "CompilerException.h"

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

		CppEmitter::CppEmitter()	
		{
			_buffer.precision(17);
		}
		
		CppEmitter& CppEmitter::Space()
		{
			_buffer << " ";
			return *this;
		}

		CppEmitter& CppEmitter::NewLine()
		{
			_buffer << std::endl;
			return *this;
		}

		CppEmitter& CppEmitter::Semicolon()
		{
			_buffer << ";";
			return *this;
		}

		CppEmitter& CppEmitter::Comma()
		{
			_buffer << ",";
			return *this;
		}

		CppEmitter& CppEmitter::OpenBrace()
		{
			_buffer << "{";
			return *this;
		}
		
		CppEmitter& CppEmitter::CloseBrace()
		{
			_buffer << "}";
			return *this;
		}

		CppEmitter& CppEmitter::OpenParan()
		{
			_buffer << "(";
			return *this;
		}

		CppEmitter& CppEmitter::CloseParan()
		{
			_buffer << ")";
			return *this;
		}

		CppEmitter& CppEmitter::Quote()
		{
			_buffer << '"';
			return *this;
		}
		
		CppEmitter& CppEmitter::Assign()
		{
			_buffer << '=';
			return *this;
		}

		CppEmitter& CppEmitter::Operator(OperatorType op)
		{
			switch (op)
			{
				case OperatorType::Add:
				case OperatorType::AddF:
					_buffer << '+';
					break;
				case OperatorType::Subtract:
				case OperatorType::SubtractF:
					_buffer << '-';
					break;
				case OperatorType::Multiply:
				case OperatorType::MultiplyF:
					_buffer << '*';
					break;
				case emll::compiler::OperatorType::DivideS:
				case emll::compiler::OperatorType::DivideF:
					_buffer << '/';
					break;
				default:
					throw new CompilerException(CompilerError::operatorTypeNotSupported);
			}
		}

		CppEmitter& CppEmitter::Cmp(ComparisonType cmp)
		{
			switch (cmp)
			{
				case ComparisonType::Eq:
				case ComparisonType::EqF:
					_buffer << "==";
					break;
				case ComparisonType::Lt:
				case ComparisonType::LtF:
					_buffer << '<';
					break;
				case ComparisonType::Lte:
				case ComparisonType::LteF:
					_buffer << "<=";
					break;
				case ComparisonType::Gt:
				case ComparisonType::GtF:
					_buffer << '>';
					break;
				case ComparisonType::Gte:
				case ComparisonType::GteF:
					_buffer << ">=";
					break;
				case ComparisonType::Neq:
				case ComparisonType::NeqF:
					_buffer << "!=";
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

		CppEmitter& CppEmitter::Token(const std::string& token)
		{
			_buffer << token;
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
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
			return *this;
		}

		CppEmitter& CppEmitter::Literal(const std::string& value)
		{
			Quote();
			_buffer << value;
			Quote();
			return *this;
		}
		
		template<>
		CppEmitter& CppEmitter::Literal<double>(double value)
		{
			_buffer << value;
			return *this;
		}

		template<>
		CppEmitter& CppEmitter::Literal<int>(int value)
		{
			_buffer << value;
			return *this;
		}

		CppEmitter& CppEmitter::Var(const ValueType type, const std::string& name)
		{
			return Type(type)
				.Space()
				.Identifier(name);
		}

		template<>
		CppEmitter& CppEmitter::Var<double>(const std::string& name)
		{
			return Var(ValueType::Double, name);
		}

		template<>
		CppEmitter& CppEmitter::Var<int>(const std::string& name)
		{
			return Var(ValueType::Int32, name);
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
					Comma();
				}
				Var(args[i]);
			}
			return *this;
		}

		CppEmitter& CppEmitter::DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args)
		{
			return Type(returnType)
					.Space()
					.Token(name)
					.OpenParan()
					.Vars(args)
					.CloseParan();
		}

		CppEmitter& CppEmitter::Clear()
		{
			_buffer.clear();
			return *this;
		}

		CppEmitter& CppEmitter::AppendRaw(const std::string& code)
		{
			_buffer << code;
			return *this;
		}
	}
}