////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppEmitter.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Types.h"
#include "IndentedTextWriter.h"

namespace emll
{
	namespace compiler
	{
		class CppEmitter
		{
		public:
			CppEmitter();

			CppEmitter& Space();
			CppEmitter& NewLine();
			CppEmitter& Semicolon();
			CppEmitter& Comma();
			CppEmitter& OpenBrace();
			CppEmitter& CloseBrace();
			CppEmitter& OpenParan();
			CppEmitter& CloseParan();
			CppEmitter& OpenBracket();
			CppEmitter& CloseBracket();
			CppEmitter& Quote();
			CppEmitter& Assign();
			CppEmitter& Asterisk();
			CppEmitter& Increment();
			CppEmitter& Operator(OperatorType op);
			CppEmitter& Cmp(ComparisonType cmp);

			CppEmitter& Const();
			CppEmitter& Static();
			CppEmitter& Token(const std::string& token);
			CppEmitter& Type(const ValueType type);
			CppEmitter& Identifier(const std::string& id) { return Token(id); }

			CppEmitter& Offset(int offset);
			CppEmitter& Dimension(int size);

			CppEmitter& Literal(const std::string& value);

			template<typename T>
			CppEmitter& Literal(T value);
			template<typename T>
			CppEmitter& Literal(const std::vector<T>& value);

			CppEmitter& Var(const ValueType type, const std::string& name);
			template<typename T>
			CppEmitter& Var(const std::string& name);
			template<typename T>
			CppEmitter& Var(const std::string& name, int size);

			CppEmitter& Var(const NamedValueType& var);
			CppEmitter& Vars(const NamedValueTypeList& vars);
			CppEmitter& DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args);

			template<typename T>
			CppEmitter& Cmp(const std::string& varName, ComparisonType cmp, T value);

			CppEmitter& Assign(const std::string& varName);
			CppEmitter& AssignValueAt(const std::string& varName, const int offset);

			CppEmitter& For();
			CppEmitter& If();
			CppEmitter& Else();

			CppEmitter& BeginBlock();
			CppEmitter& EndBlock();

			CppEmitter& AppendRaw(const std::string& code);
			CppEmitter& Clear();

			///<summary>Increase the indent</summary>
			CppEmitter& IncreaseIndent();
			///<summary>Decrease the indent</summary>
			CppEmitter& DecreaseIndent();

			std::string Code() { return _writer.ToString(); }

		private:
			IndentedTextWriter _writer;
		};
	}
}

#include "../tcc/CppEmitter.tcc"
