#pragma once

#include "Types.h"
#include "IntegerStack.h"
#include <string>
#include <vector>

namespace emll
{
	namespace compiler
	{
		struct EmittedVar
		{
			bool isNew;
			uint64_t varIndex;

			void Clear();
		};

		class EmittedVarAllocator
		{
		public:
			EmittedVar Alloc();
			void Free(EmittedVar& var);

		private:
			utilities::IntegerStack _varStack;
		};

		enum class VariableScope
		{
			Local,
			Global,
			Heap
		};

		enum class VariableType
		{
			Scalar,
			Vector
		};

		class IRCompiler;

		class Variable
		{
		public:
			enum class VariableFlags
			{
				none = 0,
				isMutable = 0x00000001,
				isVectorRef = 0x00000002,
				isComputed = 0x00000004,
			};

		public:
			ValueType Type() const
			{
				return _type;
			}
			VariableScope Scope() const
			{
				return _scope;
			}
			virtual size_t Dimension() const
			{
				return 1;
			}
			bool IsVector() const
			{
				return (Dimension() > 1);
			}
			bool IsScalar() const
			{
				return (Dimension() == 1);
			}
			const std::string& EmittedName() const
			{
				return _emittedName;
			}
			bool HasEmittedName() const
			{
				return (_emittedName.length() > 0);
			}
			void SetEmittedName(std::string emittedName);
			bool IsMutable() const
			{
				return TestFlags(VariableFlags::isMutable);
			}
			bool IsConstant() const
			{
				return !IsMutable();
			}		
			bool IsVectorRef() const
			{
				return TestFlags(VariableFlags::isVectorRef);
			}
			bool IsComputed() const
			{
				return TestFlags(VariableFlags::isComputed);
			}
			bool TestFlags(const VariableFlags flag) const
			{
				return ((_flags & (int)flag) != 0);
			}

		protected:

			Variable(const ValueType type, const VariableScope scope, const VariableFlags flags = VariableFlags::none);
			void SetFlags(const VariableFlags flag)
			{
				_flags |= (int)flag;
			}
			void ClearFlags(const VariableFlags flag)
			{
				_flags &= (~((int)flag));
			}

		private:
			std::string _emittedName;
			ValueType _type;
			VariableScope _scope;
			int _flags;
		};

		template<typename T>
		class VectorVar : public Variable
		{
		public:
			VectorVar(const VariableScope scope);

			T& Data()
			{
				return _data;
			}
			virtual size_t Dimension() const override
			{
				return _data.size();
			}
		private:
			std::vector<T> _data;
		};
		
		using VectorF = VectorVar<double>;
	}
}

#include "../tcc/Variable.tcc"

