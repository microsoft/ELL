////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variable.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variable.h"
#include "ScalarVar.h"
#include "VectorVar.h"

namespace emll
{
	namespace compiler
	{
		Variable::Variable(const ValueType type, const VariableScope scope, int flags)
			: _type(type), _scope(scope), _flags(flags)
		{
		}

		void Variable::SetEmittedName(std::string name)
		{
			_emittedName = std::move(name);
		}

		Variable* Variable::Combine(VariableAllocator& vAlloc, Variable& other, OperatorType op)
		{
			if (other.IsLiteral())
			{
				return other.Combine(vAlloc, *this, op);
			}
			return nullptr;
		}

		void EmittedVar::Clear()
		{
			isNew = false;
			varIndex = 0;
		}

		EmittedVar EmittedVarAllocator::Alloc()
		{
			EmittedVar var;
			var.isNew = _varStack.IsTopNovel();
			var.varIndex = _varStack.Pop();
			return var;
		}

		void EmittedVarAllocator::Free(EmittedVar& var)
		{
			_varStack.Push(var.varIndex);
			var.Clear();
		}

		Variable* VariableAllocator::AddLocalScalarVariable(VariableScope scope, ValueType type)
		{
			switch (type)
			{
				case ValueType::Double:
					return AddVariable<ScalarVar<double>>(scope);
				case ValueType::Int32:
					return AddVariable<ScalarVar<int>>(scope);
				case ValueType::Byte:
					return AddVariable<ScalarVar<uint8_t>>(scope);
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
		}

		Variable* VariableAllocator::AddVectorVariable(VariableScope scope, ValueType type, int size)
		{
			switch (type)
			{
				case ValueType::Double:
					return AddVariable<VectorVar<double>>(scope, size);
				case ValueType::Int32:
					return AddVariable<VectorVar<int>>(scope, size);
				case ValueType::Byte:
					return AddVariable<VectorVar<uint8_t>>(scope, size);
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}

		}

		Variable* VariableAllocator::AddVectorElementVariable(ValueType type, Variable& src, int offset)
		{
			switch (type)
			{
				case ValueType::Double:
					return AddVariable<VectorElementVar<double>>(src, offset);
				case ValueType::Int32:
					return AddVariable<VectorElementVar<int>>(src, offset);
				case ValueType::Byte:
					return AddVariable<VectorElementVar<uint8_t>>(src, offset);
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
		}
	}
}
