#include <string>

namespace emll
{
	namespace compiler
	{
		template<typename T>
		ScalarVar<T>::ScalarVar(const VariableScope scope, const VariableFlags flags)
			: Variable(GetValueType<T>(), scope, flags)
		{
		}

		static const std::string c_globalVarPrefix = "g_";
		static std::string c_tempVarPrefix = "t_";
		
		template<typename T>
		void ScalarVar<T>::AssignVar(EmittedVar var)
		{
			_emittedVar = var;
			switch (Scope())
			{
				case VariableScope::Local:
					SetEmittedName(c_tempVarPrefix + std::to_string(var.varIndex));
					break;
				case VariableScope::Global:
					SetEmittedName(c_globalVarPrefix + std::to_string(var.varIndex));
					break;
				default:
					throw new CompilerException(CompilerError::variableScopeNotSupported);
			}
		}

		template<typename T>
		EmittedVar ScalarVar<T>::GetAssignedVar()
		{
			return _emittedVar;
		}

		template<typename T>
		LiteralVar<T>::LiteralVar(T data)
			: ScalarVar(VariableScope::Literal, VariableFlags::none), 
			 _data(data)
		{
		}

		template<typename T>
		InitializedScalarVar<T>::InitializedScalarVar(const VariableScope scope, T data, bool isMutable)
			: ScalarVar(scope, isMutable ? VariableFlags::isMutable : VariableFlags::none), 
			 _data(data)
		{
		}

		template<typename T>
		ComputedVar<T>::ComputedVar()
			: Variable(GetValueType<T>(), VariableScope::Local)
		{
		}

		template<typename T>
		VectorRefScalarVar<T>::VectorRefScalarVar(VariableScope srcScope, std::string sourceName, int offset)
			: ScalarVar(VariableScope::Local, VariableFlags::isVectorRef),
			_srcScope(srcScope),
			_srcName(std::move(sourceName)),
			_offset(offset)
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