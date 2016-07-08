#pragma once
#include <vector>

namespace emll  
{ 
	namespace compiler 
	{ 
		enum class ValueType
		{
			Void = 0,
			Byte,
			Short,
			Int32,
			Int64,
			Double,
			Char8,
			//
			// Pointers
			//
			PVoid,
			PByte,
			PShort,
			PInt32,
			PInt64,
			PDouble,
			PChar8
		};

		enum class OperatorType
		{
			Add,
			Subtract,
			Multiply,
			DivideS,
			AddF,
			SubtractF,
			MultiplyF,
			DivideF,
		};

		enum class ComparisonType
		{
			Eq,
			Lt,
			Lte,
			Gt,
			Gte,
			Neq,
			EqF,
			LtF,
			LteF,
			GtF,
			GteF,
			NeqF
		};


		template<typename T>
		class TypeList : public std::vector<T>
		{
		public:
			using std::vector<T>::vector;

			void init(std::initializer_list<T> args)
			{
				clear();
				for (auto v = args.begin(); v != args.end(); ++v)
				{
					push_back(*v);
				}
			}
		};

		using NamedValueType = std::pair<std::string, ValueType>;
		using ValueTypeList = TypeList<ValueType>;
		using NamedValueTypeList = TypeList<NamedValueType>;	
	}
}
