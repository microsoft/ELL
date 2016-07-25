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
		private:
			int _size;
		};
		using VectorF = VectorVar<double>;

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
	}
}

#include "../tcc/VectorVar.tcc"