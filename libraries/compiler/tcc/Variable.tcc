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
	}
}
