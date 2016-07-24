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

			virtual Variable* Combine(VariableAllocator& vAlloc, Variable& other,  OperatorType op) override;

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
			ComputedVar(Variable& src);
			ComputedVar(ComputedVar<T>& src);

			Variable& Src() const
			{
				return _src;
			}
			T IncrementBy() const
			{
				return _add;
			}
			T MultiplyBy() const
			{
				return _multiply;
			}
			bool HasSameSource(ComputedVar<T>& other);

			void Append(T data, OperatorType op);

			virtual Variable* Combine(VariableAllocator& vAlloc, Variable& other, OperatorType op) override;
			//bool Combine(ComputedVar& other);

		private:
			void Append(ComputedVar<T>& other);
			
		private:
			Variable& _src;
			T _add;
			T _multiply;
		};
		using ComputedVarF = ComputedVar<double>;
	}
}

#include "../tcc/ScalarVar.tcc"