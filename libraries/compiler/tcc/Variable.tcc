#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		template<typename T>
		VectorVar<T>::VectorVar(const VariableScope scope)
			: Variable(GetValueType<T>(), scope)
		{
		}

		template<typename T>
		VectorRefVar<T>::VectorRefVar(std::string name)
			: Variable(GetValueType<T>(), VariableScope::Local, VariableFlags::isVectorRef), _sourceName(std::move(name))
		{
		}
	}
}
