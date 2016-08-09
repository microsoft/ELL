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
			CppEmitter& Quote();
			CppEmitter& Assign();
			CppEmitter& Asterisk();
			CppEmitter& Operator(OperatorType op);
			CppEmitter& Cmp(ComparisonType cmp);

			CppEmitter& Token(const std::string& token);
			CppEmitter& Const();
			CppEmitter& Static();

			CppEmitter& Type(const ValueType type);
			CppEmitter& Identifier(const std::string& id) { return Token(id); }

			CppEmitter& Literal(const std::string& value);

			template<typename T>
			CppEmitter& Literal(T value);

			CppEmitter& Var(const ValueType type, const std::string& name);
			template<typename T>
			CppEmitter& Var(const std::string& name);

			CppEmitter& Var(const NamedValueType& var);
			CppEmitter& Vars(const NamedValueTypeList& vars);
			CppEmitter& DeclareFunction(const std::string& name, const ValueType returnType, const NamedValueTypeList& args);

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
