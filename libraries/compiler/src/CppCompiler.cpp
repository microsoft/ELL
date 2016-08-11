////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppCompiler.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CppCompiler.h"
#include "Files.h"

namespace emll
{
	namespace compiler
	{
		static const std::string c_LoopVarName = "i";

		CppCompiler::CppCompiler()
		{
		}

		void CppCompiler::WriteToFile(const std::string& filePath)
		{
			auto fileStream = utilities::OpenOfstream(filePath);
			_module.Write(fileStream);
		}

		void CppCompiler::CompileDotProductNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileAccumulatorNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileDelayNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileUnaryNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileBinaryPredicateNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileElementSelectorNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::BeginFunction(const std::string& functionName, NamedValueTypeList& args)
		{
			_pfn = _module.Function(functionName, ValueType::Void, args);
		}
		void CppCompiler::EndFunction()
		{
			if (_pfn != nullptr)
			{
				_pfn->End();
			}
		}

		void CppCompiler::EnsureEmitted(Variable& var)
		{
			if (var.HasEmittedName())
			{
				return;
			}
			AllocVar(var);
			if (var.IsNew())
			{
				Emit(var);
			}
		}

		Variable* CppCompiler::EnsureEmitted(model::OutputPortBase* pPort)
		{
			assert(pPort != nullptr);
			Variable* pVar = GetVariableFor(pPort);
			if (pVar == nullptr)
			{
				pVar = AllocVar(pPort);
			}
			assert(pVar != nullptr);
			EnsureEmitted(pVar);
			return pVar;
		}

		Variable* CppCompiler::EnsureEmitted(model::OutputPortElement elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			EnsureEmitted(pVar);
			return pVar;
		}

		Variable* CppCompiler::EnsureEmitted(model::InputPortBase* pPort)
		{
			assert(pPort != nullptr);
			return EnsureEmitted(pPort->GetOutputPortElement(0));
		}

		void CppCompiler::Emit(Variable& var)
		{
			assert(var.HasEmittedName());
			switch (var.Type())
			{
				case ValueType::Double:
					return Emit<double>(var);
				case ValueType::Byte:
					return Emit<uint8_t>(var);
				case ValueType::Int32:
					return Emit<int>(var);
				case ValueType::Int64:
					return Emit<int64_t>(var);
				default:
					break;
			}
			throw new CompilerException(CompilerError::variableTypeNotSupported);
		}

		void CppCompiler::LoadVar(const model::OutputPortElement elt)
		{
			Variable* pVar = EnsureVariableFor(elt);
			EnsureEmitted(pVar);
			if (pVar->IsScalar())
			{
				if (elt.GetIndex() > 0)
				{
					throw new CompilerException(CompilerError::vectorVariableExpected);
				}
				if (!pVar->IsLiteral())
				{
					_pfn->Value(pVar->EmittedName());
				}
				return;
			}

			if (elt.GetIndex() >= pVar->Dimension())
			{
				throw new CompilerException(CompilerError::indexOutOfRange);
			}
			_pfn->ValueAt(pVar->EmittedName(), (int)elt.GetIndex());
		}

		void CppCompiler::SetVar(Variable& var, int offset)
		{
			if (var.IsScalar())
			{
				if (offset > 0)
				{
					throw new CompilerException(CompilerError::indexOutOfRange);
				}
				_pfn->Assign(var.EmittedName());
				return;
			}
			if (offset >= var.Dimension())
			{
				throw new CompilerException(CompilerError::indexOutOfRange);
			}
			_pfn->AssignValueAt(var.EmittedName(), offset);
		}

		const std::string& CppCompiler::LoopVarName()
		{
			return c_LoopVarName;
		}
	}
}
