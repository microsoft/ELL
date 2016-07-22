#include "Types.h"

namespace emll
{
	namespace compiler
	{
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
		ValueType GetValueType<int>()
		{
			return ValueType::Int32;
		}
		template<>
		ValueType GetValueType<int*>()
		{
			return ValueType::PInt32;
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
	}
}