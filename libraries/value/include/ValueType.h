////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueType.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace value
{

/// <summary> An enumeration of primitive types supported by the value library </summary>
enum class ValueType
{
    ///<summary> undefined type </summary>
    Undefined = -1,
    ///<summary> void type </summary>
    Void = 0,
    ///<summary> 1 byte boolean </summary>
    Boolean,
    ///<summary> 1 byte character </summary>
    Char8,
    ///<summary> 1 byte unsigned integer </summary>
    Byte,
    ///<summary> 2 byte signed integer </summary>
    Int16,
    ///<summary> 4 byte signed integer </summary>
    Int32,
    ///<summary> 8 byte signed integer </summary>
    Int64,
    ///<summary> 4 byte floating point </summary>
    Float,
    ///<summary> 8 byte floating point </summary>
    Double,
};

/// <summary> An enumeration of unary operations supported by the value library </summary>
enum class ValueUnaryOperation {};

/// <summary> An enumeration of binary operations supported by the value library </summary>
enum class ValueBinaryOperation
{
    /// <summary> Addition operation </summary>
    add,
    /// <summary> Subtraction operation </summary>
    subtract,
    /// <summary> Multiplication operation </summary>
    multiply,
    /// <summary> Division operation </summary>
    divide
};

} // value
} // ell