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
			ScalarVar(const VariableScope scope, int flags = VariableFlags::isMutable);

			virtual void AssignVar(EmittedVar var) override;
			virtual EmittedVar GetAssignedVar() override;

		private:
			EmittedVar _emittedVar;
		};
		using ScalarF = ScalarVar<double>;

		template<typename T>
		class LiteralVar : public ScalarVar<T>
		{
		public:
			LiteralVar(T data);

			T& Data()
			{
				return _data;
			}

		private:
			T _data;
		};
		using LiteralF = LiteralVar<double>;

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
		class VectorElementVar : public ScalarVar<T>
		{
		public:
			VectorElementVar(Variable& src, int offset);

			Variable& Src() const
			{
				return _src;
			}
			const int Offset() const
			{
				return _offset;
			}

		private:
			Variable& _src;
			int _offset;
		};
		using VectorRefScalarVarF = VectorElementVar<double>;

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