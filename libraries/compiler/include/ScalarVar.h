#pragma once

#include "Variable.h"

namespace emll
{
	namespace compiler
	{
		template<typename T>
		class ScalarVar : public Variable
		{
		public:
			ScalarVar(T data)
				: _data(data) {}

			T& Data()
			{
				return _data;
			}
		protected:
			ScalarVar(const VariableScope scope, T data);

		private:
			T _data;
		};

		template<typename T>
		class GlobalScalarVar : public ScalarVar<T>
		{
		public:
			GlobalScalarVar(T data);

			void AssignVar(uint64_t var);
		};
		using GlobalScalarF = GlobalScalarVar<double>;

		template<typename T>
		class LocalScalarVar : public ScalarVar<T>
		{
		public:
			LocalScalarVar(T data);

			virtual void AssignTempVar(TempVar var);

		private:
			TempVar _tempVar;
		};
		using LocalScalarF = LocalScalarVar<double>;

		template<typename T>
		class ComputedVar : public LocalScalarVar<T>
		{
		public:
			ComputedVar();

			OperatorType Op() const
			{
				return _op;
			}
			T Operand() const
			{
				return _value;
			}
			bool Combine(ComputedVar& other);

		private:
			std::string _sourceVarName;
			OperatorType _op;
			T _value;
		};

	}
}

#include "../tcc/ScalarVar.tcc"