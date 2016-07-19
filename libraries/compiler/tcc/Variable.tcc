#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		template<typename T>
		VectorVar<T>::VectorVar(const VariableScope scope)
			: Variable(GetValueType<T>, scope)
		{
		}
	}
}
