#include "Types.h"

namespace emll
{
	namespace compiler
	{
		template<>
		ValueType GetValueType<double>()
		{
			return ValueType::Double;
		}

		template<>
		ValueType GetValueType<int>()
		{
			return ValueType::Int32;
		}
	}
}