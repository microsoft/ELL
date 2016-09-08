////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Types.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>

namespace emll  
{ 
	namespace compiler 
	{ 
		///<summary>An enumeration of primitive types our compilers support</summary>
		enum class ValueType
		{
			Void = 0,
			///<summary>8 bit unsigned integer</summary>
			Byte,
			///<summary>16 bit signed integer</summary>
			Short,
			///<summary>32 bit signed integer</summary>
			Int32,
			///<summary>64 bit signed integer</summary>
			Int64,
			///<summary>8 byte floating point</summary>
			Double,
			///<summary>8 bit character</summary>
			Char8,
			//
			// Pointers
			//
			PVoid,
			///<summary>Pointer to a byte</summary>
			PByte,
			///<summary>Pointer to a short</summary>
			PShort,
			///<summary>Pointer to an Int32</summary>
			PInt32,
			///<summary>Pointer to an Int64</summary>
			PInt64,
			///<summary>Pointer to a Double</summary>
			PDouble,
			///<summary>Pointer to a character array</summary>
			PChar8
		};

		///<summary>An enumeration of strongly TYPED operations on numbers</summary>
		enum class OperatorType
		{
			none = 0,
			///<summary>Integer addition</summary>
			add,
			///<summary>Integer subtraction</summary>
			subtract,
			///<summary>Integer multiplication</summary>
			multiply,
			///<summary>Signed division - returns an integer</summary>
			divideS,
			///<summary>Floating point addition</summary>
			addF,
			///<summary>Floating point subtraction</summary>
			subtractF,
			///<summary>Floating point multiplication</summary>
			multiplyF,
			///<summary>Floating point division</summary>
			divideF,
		};

		///<summary>An enumeration of strongly TYPED comparisons on numbers</summary>
		enum class ComparisonType
		{
			none = 0,
			///<summary>Integer Equal</summary>
			eq,
			///<summary>Integer Less than</summary>
			lt,
			///<summary>Integer Less than equals</summary>
			lte,
			///<summary>Integer Greater than</summary>
			gt,
			///<summary>Integer Greater than equals</summary>
			gte,
			///<summary>Integer Not Equals</summary>
			neq,
			///<summary>Floating point Equal</summary>
			eqF,
			///<summary>Floating point less than </summary>
			ltF,
			///<summary>Floating point less than equals</summary>
			lteF,
			///<summary>Floating point greater than</summary>
			gtF,
			///<summary>Floating point less than equals</summary>
			gteF,
			///<summary>Floating point Not equals</summary>
			neqF
		};

		///<summary>A collection of types</summary>
		template<typename T>
		class TypeList : public std::vector<T>
		{
		public:
			using std::vector<T>::vector;
			///<summary>Convenience initializer</summary>
			void init(std::initializer_list<T> args);
		};

		///<summary>Commonly used to create named fields, arguments, variables</summary>
		using NamedValueType = std::pair<std::string, ValueType>;
		///<summary>Collections of value types</summary>
		using ValueTypeList = TypeList<ValueType>;
		///<summary>Collections of named value types</summary>
		using NamedValueTypeList = TypeList<NamedValueType>;

		//
		// These template functions produce Type information for C++ data types
		//
		///<summary>Valuetype for the given primitive type</summary>
		template<typename DataType>
		ValueType GetValueType();
		///<summary>Ptr type for the given primitive type</summary>
		ValueType GetPtrType(ValueType type);				
		///<summary>Default value for the given primitive type</summary>
		template<typename DataType>
		DataType GetDefaultForValueType();		
		///<summary>The type specific Add operator for the given primitive type</summary>
		template<typename DataType>
		OperatorType GetAddForValueType();		
		///<summary>The type specific Multiply operator for the given primitive type</summary>
		template<typename DataType>
		OperatorType GetMultiplyForValueType();
		///<summary>Does the given primitive type have a sign?</summary>
		bool IsSigned(ValueType type);
	}
}

#include "../tcc/Types.tcc"
