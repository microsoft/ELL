#pragma once
#include "Exception.h"

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

		using EmitterException = utilities::ErrorCodeException<EmitterError>;
	}
}

