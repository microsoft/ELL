#pragma once
#include "Error.h"

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			enum class EmitterError
			{
				Unexpected = 0,
				NotSupported,
				InvalidValueType,
				InvalidOperatorType,
				InvalidFunction,
				FunctionNotFound,
				WriteModuleFailed,
			};

			class EmitterException : EmllException<EmitterError>
			{
			public:
				EmitterException(EmitterError error)
					: EmllException(error)
				{
				}
			};

			class LLVMException : EmllException<std::error_code>
			{
			public:
				LLVMException(std::error_code error)
					: EmllException(error)
				{
				}
			};
		}
	}
}

