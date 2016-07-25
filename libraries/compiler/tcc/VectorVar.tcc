#include <string>

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

		template<typename T>
		LiteralVarV<T>::LiteralVarV(std::vector<T> data)
			:	VectorVar(VariableScope::Global, data.size(), VariableFlags::none),
				_data(std::move(data))
		{
		}
	}
}
