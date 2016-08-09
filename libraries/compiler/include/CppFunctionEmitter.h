#pragma once

#include "CppEmitter.h"

namespace emll
{
	namespace compiler
	{
		class CppFunctionEmitter
		{
		public:
			CppFunctionEmitter();

			CppFunctionEmitter& Begin(const std::string& name, const ValueType returnType, const NamedValueTypeList& args);
			CppFunctionEmitter& End();

			std::string& Code() { return _emitter.Code(); }

		private:
			CppEmitter _emitter;
		};
	}
}