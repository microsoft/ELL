#pragma once
#include "Exception.h"

namespace emll
{
	namespace compiler
	{
		///<summary>A list of error codes thrown by the compiler</summary>
		enum class CompilerError
		{
			/// Unexpected error
			unexpected = 0,					
			/// General not supported error
			notSupported,
			/// Model node type not supported
			modelNodeTypeNotSupported,
			/// Port Type not supported
			portTypeNotSupported,
			/// Binary Operators - operation not supported
			binaryOperationTypeNotSupported,
			/// Unary Operators - operation not supported
			unaryOperationNotSupported,
			/// Binary predicates - operation not supported
			binaryPredicateTypeNotSupported,
			/// Expected a scalar inputs, but a vector was encountered
			scalarInputsExpected,
			/// Expected a scalar inputs, but a vector was encountered
			scalarOutputsExpected,
			/// Unknown VariableScope
			variableScopeNotSupported,
			/// Unknown Variable type
			variableTypeNotSupported,
			/// Unknown ValueType
			valueTypeNotSupported,
			/// Unknown OperatorType
			operatorTypeNotSupported,
			/// Unknown ComparisonType
			comparisonTypeNotSupported,
			/// Could not find a variable assigned to an output port
			variableForOutputNotFound,
			/// Expected a vector variable, but a scalar was encountered
			vectorVariableExpected,
			/// Invalid index into a vector
			indexOutOfRange,
			// Function not found
			functionNotFound,
			// Write to output stream failed
			writeStreamFailed,
		};

		using CompilerException = utilities::ErrorCodeException<CompilerError>;
	}
}

