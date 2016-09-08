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
			///<summary>Construct a new scalar variable</summary>
			ScalarVar(const VariableScope scope, int flags = VariableFlags::isMutable);

			///<summary>Returns true</summary>
			virtual bool IsScalar() const override { return true;}
		};

		///<summary>A literal scalar</summary>
		template<typename T>
		class LiteralVar : public ScalarVar<T>
		{
		public:
			///<summary>Construct a new literal</summary>
			LiteralVar(T data);

			///<summary>The literal value</summary>
			const T& Data() const { return _data; }

		private:
			T _data;
		};

		///<summary>A scalar variable with an initial value</summary>
		template<typename T>
		class InitializedScalarVar : public ScalarVar<T>
		{
		public:
			///<summary>Construct a scalar that will have the given initial value</summary>
			InitializedScalarVar(const VariableScope scope, T data, bool isMutable = true);

			///<summary>Initial value for this scalar</summary>
			const T& Data() const { return _data;}

		private:
			T _data;
		};

		///<summary>A scalar variable that is a reference into a vector variable</summary>
		template<typename T>
		class VectorElementVar : public ScalarVar<T>
		{
		public:
			///<summary>Construct a vector element</summary>
			VectorElementVar(Variable& src, int offset);

			///<summary>The source vector this is an offset into</summary>
			Variable& Src() const { return _src;}

			///<summary>Offset into vector</summary>
			const int Offset() const { return _offset;}

		private:
			Variable& _src;
			int _offset;
		};
	}
}

#include "../tcc/ScalarVar.tcc"