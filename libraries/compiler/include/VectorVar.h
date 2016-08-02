#pragma once

#include "Variable.h"

namespace emll
{
	namespace compiler
	{
		template<typename T>
		class VectorVar : public Variable
		{
		public:
			VectorVar(const VariableScope scope, const size_t size, int flags = VariableFlags::isMutable);

			virtual size_t Dimension() const override
			{
				return _size;
			}
			virtual bool IsScalar() const override
			{
				return false;
			}

		private:
			int _size;
		};

		template<typename T>
		class InitializedVectorVar : public VectorVar<T>
		{
		public:
			InitializedVectorVar(const VariableScope scope, const std::vector<T>& data, int flags = VariableFlags::isMutable);

			std::vector<T>& Data()
			{
				return _data;
			}

		private:
			std::vector<T> _data;
		};

		using InitializedVectorF = InitializedVectorVar<double>;

		template<typename T>
		class LiteralVarV : public VectorVar<T>
		{
		public:
			LiteralVarV(std::vector<T> data);

			const std::vector<T>& Data() const { return _data; }

		private:
			std::vector<T> _data;
		};
		using LiteralVF = LiteralVarV<double>;
		using LiteralVI = LiteralVarV<int>;
	}
}

#include "../tcc/VectorVar.tcc"