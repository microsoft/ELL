////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VectorVar.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>

namespace emll
{
	namespace compiler
	{
		template<typename T>
		VectorVar<T>::VectorVar(const VariableScope scope, const size_t size, int flags)
			: Variable(GetValueType<T>(), scope, flags),
			_size(size)
		{
		}

		template<typename T>
		InitializedVectorVar<T>::InitializedVectorVar(const VariableScope scope, const std::vector<T>& data, int flags)
			: VectorVar(scope, data.size(), flags | VariableFlags::hasInitValue),
			_data(std::move(data))
		{
		}

		template<typename T>
		LiteralVarV<T>::LiteralVarV(std::vector<T> data)
			:	VectorVar(VariableScope::Literal, data.size(), VariableFlags::none),
				_data(std::move(data))
		{
		}
	}
}
