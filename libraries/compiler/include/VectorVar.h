////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VectorVar.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Variable.h"

namespace emll
{
	namespace compiler
	{
		///<summary>A variable that represents a vector (array)</summary>
		template<typename T>
		class VectorVar : public Variable
		{
		public:
			///<summary>Create a new vector variable</summary>
			VectorVar(const VariableScope scope, const size_t size, int flags = VariableFlags::isMutable);

			///<summary>The size of the vector</summary>
			virtual size_t Dimension() const override { return _size;}

			///<summary>Is this variable a scalar? Returns false. </summary>
			virtual bool IsScalar() const override { return false;}

		private:
			int _size;
		};

		///<summary>A vector variable that is intialized with some the given vector data</summary>
		template<typename T>
		class InitializedVectorVar : public VectorVar<T>
		{
		public:
			///<summary>Create a new vector variable initialized with the given data</summary>
			InitializedVectorVar(const VariableScope scope, const std::vector<T>& data, int flags = VariableFlags::isMutable);

			///<summary>The data this vector is initialized with</summary>
			const std::vector<T>& Data() const
			{
				return _data;
			}

		private:
			std::vector<T> _data;
		};

		///<summary>A vector variable that is typically emitted as a static const or global - depending on the language</summary>
		template<typename T>
		class LiteralVarV : public VectorVar<T>
		{
		public:
			///<summary>Create a new literal using the given data</summary>
			LiteralVarV(std::vector<T> data);

			///<summary>The data this vector is initialized with</summary>
			const std::vector<T>& Data() const { return _data; }

		private:
			std::vector<T> _data;
		};
	}
}

#include "../tcc/VectorVar.tcc"