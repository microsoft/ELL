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
			notSupported,
			portTypeNotSupported,
			InputOutputPortTypeMismatch,
			inputNodeExpected,
			InvalidValueType,
			InvalidOperatorType,
			InvalidComparisonType,
			variableHasNoName,
			variableScopeNotSupported,
			variableTypeNotSupported,
			valueTypeNotSupported,
			valueTypeMismatch,
			InvalidFunction,
			FunctionNotFound,
			WriteStreamFailed,
		};

		using CompilerException = utilities::ErrorCodeException<CompilerError>;
	}
}

