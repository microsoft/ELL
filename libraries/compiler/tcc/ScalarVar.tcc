#include <string>

namespace emll
{
	namespace compiler
	{
		template<typename T>
		ScalarVar<T>::ScalarVar(const VariableScope scope, int flags)
			: Variable(GetValueType<T>(), scope, flags)
		{
		}

		template<typename T>
		LiteralVar<T>::LiteralVar(T data)
			: ScalarVar(VariableScope::Literal, VariableFlags::none), 
			 _data(data)
		{
		}

		template<typename T>
		Variable* LiteralVar<T>::Combine(VariableAllocator& vAlloc, Variable& other, OperatorType op)
		{
			if (Type() != other.Type())
			{
				return nullptr;
			}
			ComputedVar<T> *pVar = nullptr;
			if (other.IsComputed())
			{
				pVar = &(static_cast<ComputedVar<T>&>(other));
			}
			else
			{
				pVar = vAlloc.AddVariable<ComputedVar<T>>(other);
			}
			pVar->Append(_data, op);
			return pVar;
		}

		template<typename T>
		InitializedScalarVar<T>::InitializedScalarVar(const VariableScope scope, T data, bool isMutable)
			: ScalarVar(scope, isMutable ? (VariableFlags::isMutable | VariableFlags::hasInitValue) : VariableFlags::hasInitValue),
			 _data(data)
		{
		}

		template<typename T>
		VectorElementVar<T>::VectorElementVar(Variable& src, int offset)
			: ScalarVar(VariableScope::Local, VariableFlags::isVectorRef),
			_src(src),
			_offset(offset)
		{
		}

		template<typename T>
		ComputedVar<T>::ComputedVar(Variable& src)
			: ScalarVar(VariableScope::Local, VariableFlags::isComputed),
			_src(src),
			_add(0),
			_multiply(1)
		{
		}

		template<typename T>
		ComputedVar<T>::ComputedVar(ComputedVar<T>& src)
			: ScalarVar(src.Scope(), VariableFlags::isComputed),
			 _src(src._src),
			 _add(src._add),
			 _multiply(src._multiply)
		{
		}

		template<typename T>
		bool ComputedVar<T>::HasSameSource(ComputedVar<T>& other)
		{
			return (&(_src) == &(other._src));
		}

		template<typename T>
		Variable* ComputedVar<T>::Combine(VariableAllocator& vAlloc, Variable& other, OperatorType op)
		{
			if (other.IsLiteral())
			{
				return static_cast<LiteralVar<T>&>(other).Combine(vAlloc, *this, op);
			}
			if (!other.IsComputed() || Type() != other.Type())
			{
				return nullptr;
			}
			
			ComputedVar<T>& computed = static_cast<ComputedVar<T>&>(other);
			if (!HasSameSource(computed))
			{
				return nullptr;
			}

			ComputedVar<T> *pVar = vAlloc.AddVariable<ComputedVar<T>>(computed);
			pVar->Append(*this);
			return pVar;
		}

		template<typename T>
		void ComputedVar<T>::Append(T data, OperatorType op)
		{
			switch (op)
			{
				case OperatorType::Add:
				case OperatorType::AddF:
					_add += data;
					break;
				case OperatorType::Subtract:
				case OperatorType::SubtractF:
					_add -= data;
					break;
				case OperatorType::Multiply:
				case OperatorType::MultiplyF:
					_multiply *= data;
					break;
				case OperatorType::DivideF:
					_multiply /= data;
					break;
				default:
					throw new CompilerException(CompilerError::notSupported);
			}
		}

		template<typename T>
		void ComputedVar<T>::Append(ComputedVar<T>& other)
		{
			_add += other._add;
			_multiply *= other._multiply;
		}
	}
}