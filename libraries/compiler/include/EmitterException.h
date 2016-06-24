#pragma once
#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		enum class EmitterError
		{
			Unexpected = 0,
			NotSupported,
			InvalidValueType,
			InvalidOperatorType,
			InvalidComparisonType,
			InvalidFunction,
			FunctionNotFound,
			WriteStreamFailed,
		};

		class EmitterException : CompilerException<EmitterError>
		{
		public:
			EmitterException(EmitterError error)
				: CompilerException(error)
			{
			}
		};
	}
}

