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
			None = 0,
			///<summary>Integer addition</summary>
			Add,
			///<summary>Integer subtraction</summary>
			Subtract,
			///<summary>Integer multiplication</summary>
			Multiply,
			///<summary>Signed division - returns an integer</summary>
			DivideS,
			///<summary>Floating point addition</summary>
			AddF,
			///<summary>Floating point subtraction</summary>
			SubtractF,
			///<summary>Floating point multiplication</summary>
			MultiplyF,
			///<summary>Floating point division</summary>
			DivideF,
		};

		///<summary>An enumeration of strongly TYPED comparisons on numbers</summary>
		enum class ComparisonType
		{
			None = 0,
			///<summary>Integer Equal</summary>
			Eq,
			///<summary>Integer Less than</summary>
			Lt,
			///<summary>Integer Less than equals</summary>
			Lte,
			///<summary>Integer Greater than</summary>
			Gt,
			///<summary>Integer Greater than equals</summary>
			Gte,
			///<summary>Integer Not Equals</summary>
			Neq,
			///<summary>Floating point Equal</summary>
			EqF,
			///<summary>Floating point less than </summary>
			LtF,
			///<summary>Floating point less than equals</summary>
			LteF,
			///<summary>Floating point greater than</summary>
			GtF,
			///<summary>Floating point less than equals</summary>
			GteF,
			///<summary>Floating point Not equals</summary>
			NeqF
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
