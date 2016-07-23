#include "Variable.h"
#include "IRCompiler.h"

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

		Variable* VariableAllocator::AddLocalScalarVariable(ValueType type)
		{
			switch (type)
			{
			case ValueType::Double:
				return AddVariable<ScalarF>(VariableScope::Local);
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
			default:
				throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
		}
	}
}
