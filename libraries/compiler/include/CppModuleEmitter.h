#pragma once

#include "CppEmitter.h"

namespace emll
{
	namespace compiler
	{
		class CppModuleEmitter
		{
		public:
			template<typename T>
			CppModuleEmitter& Global(const std::string& name, bool needsInit = false);
			template<typename T>
			CppModuleEmitter& Global(const std::string& name, T value);

			CppModuleEmitter& Constant(const ValueType type, const std::string& name);

			void Dump();

			void Write(std::ostream& os);

		private:
			CppEmitter _globals;
			CppEmitter _functionDeclarations;
			CppEmitter _functions;
		};
	}
}

#include "../tcc/CppModuleEmitter.tcc"
