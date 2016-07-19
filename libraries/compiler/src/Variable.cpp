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

		void TempVar::Clear()
		{
			isNew = false;
			varIndex = 0;
		}

		TempVar TempVarAllocator::Alloc()
		{
			TempVar var;
			var.isNew = _varStack.IsTopNovel();
			var.varIndex = _varStack.Pop();
			return var;
		}

		void TempVarAllocator::Free(TempVar& var)
		{
			_varStack.Push(var.varIndex);
			var.Clear();
		}
	}
}
