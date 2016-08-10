#pragma once

#include "CppEmitter.h"
#include <memory>

namespace emll
{
	namespace compiler
	{
		class CppFunctionEmitter;

		///<summary>Class to emit cpp code</summary>
		class CppModuleEmitter
		{
		public:
			template<typename T>
			CppModuleEmitter& Global(const std::string& name, bool needsInit = false);
			template<typename T>
			CppModuleEmitter& Global(const std::string& name, T value);
			template<typename T>
			CppModuleEmitter& GlobalV(const std::string& name, const int size);
			template<typename T>
			CppModuleEmitter& GlobalV(const std::string& name, const std::vector<T>& value);
			template<typename T>
			CppModuleEmitter& Constant(const std::string& name, T value);
			template<typename T>
			CppModuleEmitter& ConstantV(const std::string& name, const std::vector<T>& value);

			CppFunctionEmitter* Function(const std::string& name, const ValueType returnType, const NamedValueTypeList& args, bool isPublic = false);

			void Dump();

			void Write(std::ostream& os);

		private:
			CppEmitter _constants;
			CppEmitter _globals;
			std::vector<std::shared_ptr<CppFunctionEmitter>> _functions;
		};
	}
}

#include "../tcc/CppModuleEmitter.tcc"
