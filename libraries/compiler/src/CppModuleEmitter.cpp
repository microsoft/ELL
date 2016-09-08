////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppModuleEmitter.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CppModuleEmitter.h"
#include "CppFunctionEmitter.h"
#include <iostream>

namespace emll
{
	namespace compiler
	{
		CppFunctionEmitter* CppModuleEmitter::Function(const std::string& name, const ValueType returnType, const NamedValueTypeList& args, bool isPublic)
		{
			auto fn = std::make_shared<CppFunctionEmitter>();
			fn->Begin(name, returnType, args);
			CppFunctionEmitter* pfn = fn.get();			
			_functions.push_back(fn);
			return pfn;
		}

		void CppModuleEmitter::Dump()
		{
			Write(std::cout);
			std::cout << std::endl;
		}

		void CppModuleEmitter::Write(std::ostream& os)
		{
			os << _constants.Code() << std::endl;
			os << _globals.Code() << std::endl;
			for (auto fn : _functions)
			{
				os << fn->Code();
			}
		}
	}
}