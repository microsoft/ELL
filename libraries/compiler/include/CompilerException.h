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
			nodeTypeNotSupported,
			portTypeNotSupported,
			operationTypeNotSupported,
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
			variableForOutputNotFound,
			variableIsNotVector,
			indexOutOfRange,
			InvalidFunction,
			FunctionNotFound,
			WriteStreamFailed,
		};

		using CompilerException = utilities::ErrorCodeException<CompilerError>;
	}
}

