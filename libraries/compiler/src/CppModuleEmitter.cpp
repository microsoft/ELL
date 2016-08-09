#include "CppModuleEmitter.h"
#include "CppFunctionEmitter.h"
#include <iostream>

namespace emll
{
	namespace compiler
	{
		CppModuleEmitter& CppModuleEmitter::Constant(const ValueType type, const std::string& name)
		{			
			_globals.NewLine()
				.Const()
				.Space()
				.Var(type, name)
				.Semicolon();

			return *this;
		}

		void CppModuleEmitter::AddFunction(CppFunctionEmitter& fn)
		{
			_functions.AppendRaw(fn.Code());
		}

		void CppModuleEmitter::Dump()
		{
			Write(std::cout);
		}

		void CppModuleEmitter::Write(std::ostream& os)
		{
			os << _globals.Code() << std::endl;
			os << _functions.Code() << std::endl;
		}
	}
}