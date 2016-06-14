#pragma once
#include "Error.h"

namespace emll
{
	namespace codegen
	{
		enum EmitterError
		{
			Unexpected = 0,
			NotSupported,
			InvalidValueType,
			InvalidOperatorType,
			InvalidFunction,
			FunctionNotFound,
			WriteModuleFailed,
		};

		class EmitterException : core::EmllException<EmitterError>
		{
		public:
			EmitterException(EmitterError error) 
				: EmllException(error)
			{
			}
		};

		class LLVMException : core::EmllException<std::error_code>
		{
		public:
			LLVMException(std::error_code error)
				: EmllException(error)
			{
			}
		};
	}
}

