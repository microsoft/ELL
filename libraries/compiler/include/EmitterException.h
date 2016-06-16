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

			class EmitterException : CompilerException<EmitterError>
			{
			public:
				EmitterException(EmitterError error)
					: CompilerException(error)
				{
				}
			};

			class LLVMException : CompilerException<std::error_code>
			{
			public:
				LLVMException(std::error_code error)
					: CompilerException(error)
				{
				}
			};
		}
	}
}

