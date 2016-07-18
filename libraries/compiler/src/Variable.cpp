#include "Variable.h"

namespace emll
{
	namespace compiler
	{
		Variable::Variable(const ValueType type, const VariableFlags flags)
			: _type(type), _flags((int) flags)
		{
		}

		void Variable::SetEmitted(std::string name)
		{
			_emittedName = std::move(name);
		}
	}
}
