#include <string>

namespace emll
{
	namespace compiler
	{
		template<typename T>
		ScalarVar<T>::ScalarVar(const VariableScope scope, T data)
			: Variable(GetValueType<T>(), scope), _data(data)
		{
		}

		const std::string c_globalVarPrefix = "g_";

		template<typename T>
		GlobalScalarVar<T>::GlobalScalarVar(T data)
			: ScalarVar(VariableScope::Global, data)
		{
		}

		template<typename T>
		void GlobalScalarVar<T>::AssignVar(uint64_t var)
		{
			SetEmittedName(c_globalVarPrefix + std::to_string(var));
		}

		const std::string c_tempVarPrefix = "t_";

		template<typename T>
		LocalScalarVar<T>::LocalScalarVar(T data)
			: ScalarVar(VariableScope::Local, data)
		{
		}

		template<typename T>
		void LocalScalarVar<T>::AssignTempVar(TempVar var)
		{
			_tempVar = std::move(var);
			SetEmittedName(c_tempVarPrefix + std::to_string(_tempVar.varIndex));
		}

		template<typename T>
		ComputedVar<T>::ComputedVar()
			: Variable(GetValueType<T>(), VariableScope::Local)
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
				throw new CompilerException(CompilerError::notSupported);
			}

			return true;
		}

	}
}