////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ScalarVar.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
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
			: ScalarVar(VariableScope::literal, VariableFlags::none), 
			 _data(data)
		{
		}

		template<typename T>
		InitializedScalarVar<T>::InitializedScalarVar(const VariableScope scope, T data, bool isMutable)
			: ScalarVar(scope, isMutable ? (VariableFlags::isMutable | VariableFlags::hasInitValue) : VariableFlags::hasInitValue),
			 _data(data)
		{
		}

		template<typename T>
		VectorElementVar<T>::VectorElementVar(Variable& src, int offset)
			: ScalarVar(VariableScope::local, VariableFlags::isVectorRef),
			_src(src),
			_offset(offset)
		{
		}
	}
}