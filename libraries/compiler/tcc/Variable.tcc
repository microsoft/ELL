#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		template<typename T>
		ScalarVar<T>::ScalarVar(T data)
			: Variable(GetValueType<T>()), _data(data)
		{
		}

		template<typename T>
		VectorVar<T>::VectorVar()
			: Variable(GetValueType<T>)
		{
		}

		template<typename T>
		ComputedVar<T>::ComputedVar()
			: Variable(GetValueType<T>())
		{

		}

		template<typename T>
		bool ComputedVar<T>::Combine(ComputedVar& other)
		{
			if (other.Type() != Type() ||
				other._op != _op)
			{
				return false;
			}

			switch (_op)
			{
				case OperatorType::Add:
				case OperatorType::AddF:
				case OperatorType::Subtract:
				case OperatorType::SubtractF:
					_value += other._value;
					break;
				case OperatorType::Multiply:
				case OperatorType::MultiplyF:
				case OperatorType::DivideF:
					_value *= other._value;
					break;
				default:
					throw new CompilerException(CompilerError::operatorTypeNotSupported);
			}

			return true;
		}
	}
}
