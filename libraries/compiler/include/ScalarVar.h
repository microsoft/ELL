////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ScalarVar.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"

namespace emll
{
	namespace compiler
	{
		///<summary>A scalar variable</summary>
		template<typename T>
		class ScalarVar : public Variable
		{
		public:
			ScalarVar(const VariableScope scope, int flags = VariableFlags::isMutable);

			virtual bool IsScalar() const override
			{
				return true;
			}

		};

		///<summary>A literal scalar</summary>
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

		///<summary>A scalar variable with an initial value</summary>
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

		///<summary>A scalar variable that is a reference into a vector variable</summary>
		template<typename T>
		class VectorElementVar : public ScalarVar<T>
		{
		public:
			VectorElementVar(Variable& src, int offset);

			///<summary>The source vector this is an offset into</summary>
			Variable& Src() const
			{
				return _src;
			}
			///<summary>Offset into vector</summary>
			const int Offset() const
			{
				return _offset;
			}

		private:
			Variable& _src;
			int _offset;
		};

		///<summary>A computed variable</summary>
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

			bool Append(T data, OperatorType op);

			virtual Variable* Combine(VariableAllocator& vAlloc, Variable& other, OperatorType op) override;

			OperatorType LastOp() const
			{
				return _lastOp;
			}
		private:
			void Append(ComputedVar<T>& other);
			bool PrepareAppend(OperatorType op);
			
		private:
			Variable& _src;
			T _add;
			T _multiply;
			OperatorType _lastOp = OperatorType::None;
			int _opCount = 0;
		};
		using ComputedVarF = ComputedVar<double>;
	}
}

#include "../tcc/ScalarVar.tcc"