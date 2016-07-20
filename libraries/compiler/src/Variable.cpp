#include "Variable.h"
#include "IRCompiler.h"

namespace emll
{
	namespace compiler
	{
		Variable::Variable(const ValueType type, const VariableScope scope, const VariableFlags flags)
			: _type(type), _scope(scope), _flags((int) flags)
		{
		}

		void Variable::SetEmittedName(std::string name)
		{
			_emittedName = std::move(name);
		}

		void Variable::AssignVar(EmittedVar var)
		{
			throw new CompilerException(CompilerError::notSupported);
		}

		EmittedVar Variable::GetAssignedVar()
		{
			throw new CompilerException(CompilerError::notSupported);
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
	}
}
