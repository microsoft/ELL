#pragma once
#include "Exception.h"

namespace emll
{
	namespace compiler
	{
		///<summary>A list of error codes thrown by the compiler</summary>
		enum class CompilerError
		{
			Unexpected = 0,
			NotSupported,
			InputPortTypeNotSupported,
			OutputPortTypeNotSupported,
			InputOutputPortTypeMismatch,
			InputNodeExpected,
			InvalidValueType,
			InvalidOperatorType,
			InvalidComparisonType,
			InvalidFunction,
			FunctionNotFound,
			WriteStreamFailed,
		};

		using CompilerException = utilities::ErrorCodeException<CompilerError>;
	}
}

