#include "Variable.h"

namespace emll
{
	namespace compiler
	{
		Variable::Variable()
		{
		}

		Variable::Variable(std::string name)
			: _name(std::move(name))
		{
		}

		void Variable::SetName(std::string name)
		{
			_name = std::move(name);
		}
	}
}
