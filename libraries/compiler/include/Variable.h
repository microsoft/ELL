#pragma once

#include "Types.h"
#include <string>
#include <vector>

namespace emll
{
	namespace compiler
	{
		class Variable
		{
		public:
			virtual ValueType Type() const
			{
				return _type;
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
			void SetEmitted(std::string emittedName);
			bool IsEmitted() const
			{
				return (_emittedName.length() > 0);
			}
			bool IsMutable() const
			{
				return IsFlagSet(VariableFlags::isMutable);
			}
			bool IsGlobal() const
			{
				return IsFlagSet(VariableFlags::isGlobal);
			}
			bool IsVectorRef() const
			{
				return IsFlagSet(VariableFlags::isOffset);
			}

		protected:
			enum class VariableFlags
			{
				none = 0,
				isMutable = 0x00000001,
				isGlobal = 0x00000002,
				isOffset = 0x00000004,
			};

			Variable(const ValueType type, const VariableFlags flags = VariableFlags::none);

			bool IsFlagSet(const VariableFlags flag) const
			{
				return ((_flags & (int) flag) != 0);
			}
			void SetFlag(const VariableFlags flag)
			{
				_flags |= (int) flag;
			}
			void ClearFlag(const VariableFlags flag)
			{
				_flags &= (~ ((int) flag));
			}

		private:
			ValueType _type;
			std::string _emittedName;
			int _flags;
		};

		template<ValueType DataType, typename T>
		class ScalarVar : public Variable
		{
		public:
			ScalarVar(T data);

			T& Data()
			{
				return _data;
			}

		private:
			T _data;
		};

		using ScalarF = ScalarVar<ValueType::Double, double>;

		template<ValueType DataType, typename T>
		class VectorVar : public Variable
		{
		public:
			VectorVar();

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
		
		using VectorF = VectorVar<ValueType::Double, double>;

		class RefVar : public Variable
		{
		private:
			std::string _sourceName;
		};

		class VectorRefVar : public RefVar
		{
		public:
		protected:
			const size_t MAX_OFFSET = UINT_MAX;
		
		private:
			size_t _offset;
		};
	}
}

#include "../tcc/Variable.tcc"

