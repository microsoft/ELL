////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppCompiler.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CppCompiler.h"

namespace emll
{
	namespace compiler
	{
		CppCompiler::CppCompiler()
		{
		}

		void CppCompiler::CompileConstantNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}

		void CppCompiler::CompileInputNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileOutputNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileBinaryNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileDotProductNode(const model::Node& node)
		{
			throw new CompilerException(CompilerError::notSupported);
		}
		void CppCompiler::CompileSumNode(const model::Node& node)
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
	}
}
