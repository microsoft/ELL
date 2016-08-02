#include "Types.h"

namespace emll
{
	namespace compiler
	{
		template<>
		ValueType GetValueType<char>()
		{
			return ValueType::Char8;
		}
		template<>
		ValueType GetValueType<char*>()
		{
			return ValueType::PChar8;
		}
		template<>
		ValueType GetValueType<bool>()
		{
			return ValueType::Byte;
		}
		template<>
		ValueType GetValueType<uint8_t>()
		{
			return ValueType::Byte;
		}
		template<>
		ValueType GetValueType<uint8_t*>()
		{
			return ValueType::PByte;
		}
		template<>
		ValueType GetValueType<short>()
		{
			return ValueType::Short;
		}
		template<>
		ValueType GetValueType<short*>()
		{
			return ValueType::PShort;
		}
		template<>
		ValueType GetValueType<int>()
		{
			return ValueType::Int32;
		}
		template<>
		ValueType GetValueType<int*>()
		{
			return ValueType::PInt32;
		}
		template<>
		ValueType GetValueType<int64_t>()
		{
			return ValueType::Int64;
		}
		template<>
		ValueType GetValueType<int64_t*>()
		{
			return ValueType::PInt64;
		}
		template<>
		ValueType GetValueType<double>()
		{
			return ValueType::Double;
		}
		template<>
		ValueType GetValueType<double*>()
		{
			return ValueType::PDouble;
		}

		template<>
		double GetDefaultForValueType<double>()
		{
			return 0.0;
		}
		template<>
		int GetDefaultForValueType<int>()
		{
			return 0;
		}

		ValueType GetPtrType(ValueType type)
		{
			switch (type)
			{
				case ValueType::Void:
					return ValueType::PVoid;
				case ValueType::Byte:
					return ValueType::PByte;
				case ValueType::Short:
					return ValueType::PShort;
				case ValueType::Int32:
					return ValueType::PInt32;
				case ValueType::Int64:
					return ValueType::PInt64;
				case ValueType::Double:
					return ValueType::PDouble;
				case ValueType::Char8:
					return ValueType::PChar8;
				default:
					break;
			}
			return type;
		}

		template<>
		OperatorType GetAddForValueType<double>()
		{
			return OperatorType::AddF;
		}
		template<>
		OperatorType GetAddForValueType<int>()
		{
			return OperatorType::Add;
		}
		template<>
		OperatorType GetMultiplyForValueType<double>()
		{
			return OperatorType::MultiplyF;
		}
		template<>
		OperatorType GetMultiplyForValueType<int>()
		{
			return OperatorType::Multiply;
		}

	}
}