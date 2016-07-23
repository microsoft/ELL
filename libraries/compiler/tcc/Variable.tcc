#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		template<typename T>
		VectorVar<T>::VectorVar(const VariableScope scope, const size_t size, int flags)
			: Variable(GetValueType<T>(), scope, flags),
			_size(size)
		{
		}

		template <typename VarType, typename... Args>
		VarType* VariableAllocator::AddVariable(Args&&... args)
		{
			static_assert(std::is_base_of<Variable, VarType>::value, "AddVariable requires you inherit from Variable");

			auto var = std::make_shared<VarType>(args...);
			VarType* pVar = var.get();
			_variables.push_back(var);
			return pVar;
		}

	}
}
