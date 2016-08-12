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
	}
}

#include "../tcc/ScalarVar.tcc"