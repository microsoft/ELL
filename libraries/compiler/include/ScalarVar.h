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
			ScalarVar(const VariableScope scope, const VariableFlags flags);

			virtual void AssignVar(EmittedVar var) override;
			virtual EmittedVar GetAssignedVar() override;

		private:
			EmittedVar _emittedVar;
		};

		template<typename T>
		class InitializedScalarVar : public ScalarVar<T>
		{
		public:
			InitializedScalarVar(const VariableScope scope, T data, bool isMutable = true);

			T& Data()
			{
				return _data;
			}

		private:
			T _data;
		};
		using InitializedScalarF = InitializedScalarVar<double>;

		template<typename T>
		class VectorRefScalarVar : public ScalarVar<T>
		{
		public:
			VectorRefScalarVar(std::string srcName, int offset, VariableScope srcScope);

			const std::string& SrcName() const
			{
				return _srcName;
			}
			const int Offset() const
			{
				return _offset;
			}
			const VariableScope SrcScope() const
			{
				return _srcScope;
			}
		private:
			VariableScope _srcScope;
			std::string _srcName;
			int _offset;
		};
		using VectorRefScalarVarF = VectorRefScalarVar<double>;

		template<typename T>
		class ComputedVar : public ScalarVar<T>
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