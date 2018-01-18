////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterTypes.h (emitter)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// stl
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace ell
{
namespace emitters
{
    ///<summary> An enumeration of primitive types our compilers support </summary>
    enum class VariableType
    {
        Void = 0,
        ///<summary> 1 bit boolean </summary>
        Boolean,
        ///<summary> 8 bit character </summary>
        Char8,
        ///<summary> 8 bit unsigned integer </summary>
        Byte,
        ///<summary> 16 bit signed integer </summary>
        Short,
        ///<summary> 32 bit signed integer </summary>
        Int32,
        ///<summary> 64 bit signed integer </summary>
        Int64,
        ///<summary> 4 byte floating point </summary>
        Float,
        ///<summary> 8 byte floating point </summary>
        Double,

        //
        // Pointers
        //
        VoidPointer,
        ///<summary> Pointer to a character array </summary>
        Char8Pointer,
        ///<summary> Pointer to a byte </summary>
        BytePointer,
        ///<summary> Pointer to a short </summary>
        ShortPointer,
        ///<summary> Pointer to an Int32 </summary>
        Int32Pointer,
        ///<summary> Pointer to an Int64 </summary>
        Int64Pointer,
        ///<summary> Pointer to a Float </summary>
        FloatPointer,
        ///<summary> Pointer to a Double </summary>
        DoublePointer

        //
        // Arrays
        //
    };

    /// <summary> Untyped unary operations. </summary>
    enum class UnaryOperationType
    {
        none,
        exp, // real only
        log, // real only
        sqrt, // real only
        logicalNot, // bool only
        tanh, // real only
        square, // real only
        sin, // real only
        cos, // real only
    };

    /// <summary> Untyped binary operations. </summary>
    enum class BinaryOperationType
    {
        none,
        add,
        subtract,
        coordinatewiseMultiply, // coordinatewise multiplication
        coordinatewiseDivide, // coordinatewise division
        logicalAnd,
        logicalOr,
        logicalXor,
        shiftLeft,
        logicalShiftRight,
        arithmeticShiftRight
    };

    /// <summary> Untyped binary predicates. </summary>
    enum class BinaryPredicateType
    {
        none,
        equal,
        less,
        greater,
        notEqual,
        lessOrEqual,
        greaterOrEqual
    };

    ///<summary> An enumeration of strongly-typed operations on numbers </summary>
    enum class TypedOperator
    {
        none = 0,
        ///<summary> Integer addition </summary>
        add,
        ///<summary> Integer subtraction </summary>
        subtract,
        ///<summary> Integer multiplication </summary>
        multiply,
        ///<summary> Integer signed division - returns an integer </summary>
        divideSigned,
        ///<summary> modulo </summary>
        moduloSigned,
        ///<summary> Floating point addition </summary>
        addFloat,
        ///<summary> Floating point subtraction </summary>
        subtractFloat,
        ///<summary> Floating point multiplication </summary>
        multiplyFloat,
        ///<summary> Floating point division </summary>
        divideFloat,
        ///<summary> Binary and </summary>
        logicalAnd,
        ///<summary> Binary or </summary>
        logicalOr,
        ///<summary> Xor </summary>
        logicalXor,
        ///<summary> Bit-shift left </summary>
        shiftLeft,
        ///<summary> Bit-shift right, padding with zeros </summary>
        logicalShiftRight,
        ///<summary> Bit-shift right, extending sign bit </summary>
        arithmeticShiftRight
    };

    ///<summary> An enumeration of strongly TYPED comparisons on numbers </summary>
    enum class TypedComparison
    {
        none = 0,
        ///<summary> Integer Equal </summary>
        equals,
        ///<summary> Integer Less than </summary>
        lessThan,
        ///<summary> Integer Less than equals </summary>
        lessThanOrEquals,
        ///<summary> Integer Greater than </summary>
        greaterThan,
        ///<summary> Integer Greater than equals </summary>
        greaterThanOrEquals,
        ///<summary> Integer Not Equals </summary>
        notEquals,
        ///<summary> Floating point Equal </summary>
        equalsFloat,
        ///<summary> Floating point less than  </summary>
        lessThanFloat,
        ///<summary> Floating point less than equals </summary>
        lessThanOrEqualsFloat,
        ///<summary> Floating point greater than </summary>
        greaterThanFloat,
        ///<summary> Floating point less than equals </summary>
        greaterThanOrEqualsFloat,
        ///<summary> Floating point Not equals </summary>
        notEqualsFloat
    };

    /// <summary> Translate the unary operation operator into a strongly typed operator for LLVM </summary>
    ///
    /// <typeparam name="T"> The type of the value to operate on. </param>
    /// <param name="operation"> The (untyped) unary operation. </param>
    ///
    /// <returns> The typed version of the operation for the given type. </param>
    template <typename T>
    TypedOperator GetOperator(emitters::UnaryOperationType operation);

    /// <summary> Translate the binary operation operator into a strongly typed operator for LLVM </summary>
    ///
    /// <typeparam name="T"> The type of the values to operate on. </param>
    /// <param name="operation"> The (untyped) binary operation. </param>
    ///
    /// <returns> The typed version of the operation for the given type. </param>
    template <typename T>
    TypedOperator GetOperator(emitters::BinaryOperationType operation);

    /// <summary> Translate the binary operation operator into a strongly typed operator for LLVM </summary>
    ///
    /// <param name="operation"> The (untyped) binary operation. </param>
    ///
    /// <returns> The typed version of the operation for floating-point types. </param>
    TypedOperator GetFloatOperator(emitters::BinaryOperationType operation);

    /// <summary> Translate the binary operation operator into a strongly typed operator for LLVM </summary>
    ///
    /// <param name="operation"> The (untyped) binary operation. </param>
    ///
    /// <returns> The typed version of the operation for integral types. </param>
    TypedOperator GetIntegerOperator(emitters::BinaryOperationType operation);

    /// <summary> Translate the binary predicate operator into a more strongly typed operator for LLVM </summary>
    ///
    /// <typeparam name="T"> The type of the values to compare. </param>
    /// <param name="predicate"> The (untyped) binary comparison. </param>
    ///
    /// <returns> The typed version of the comparison for the given type. </param>
    template <typename T>
    TypedComparison GetComparison(emitters::BinaryPredicateType predicate);

    /// <summary> Translate the binary predicate operator into a more strongly typed operator for LLVM </summary>
    ///
    /// <param name="predicate"> The (untyped) binary comparison. </param>
    ///
    /// <returns> The typed version of the comparison for floating-point types. </param>
    TypedComparison GetFloatComparison(emitters::BinaryPredicateType predicate);

    /// <summary> Translate the binary predicate operator into a more strongly typed operator for LLVM </summary>
    ///
    /// <param name="predicate"> The (untyped) binary comparison. </param>
    ///
    /// <returns> The typed version of the comparison for integral types. </param>
    TypedComparison GetIntegerComparison(emitters::BinaryPredicateType predicate);

    ///<summary> Commonly used to create named fields, arguments, variables </summary>
    using NamedVariableType = std::pair<std::string, VariableType>;

    ///<summary> Collections of variable types </summary>
    using VariableTypeList = std::vector<VariableType>;

    ///<summary> Collections of named variable types </summary>
    using NamedVariableTypeList = std::vector<NamedVariableType>;

    /// <summary> Gets the VariableType enum that corresponds to a given native C++ type. </summary>
    ///
    /// <typeparam name="ValueType"> The native C++ type being mapped to a value of VariableType. </typeparam>
    ///
    /// <returns> A VariableType that corresponds to a given native C++ type. </returns>
    template <typename ValueType>
    VariableType GetVariableType();

    /// <summary> Gets the value form the VariableType enum that corresponds to a pointer to a given nonpointer type. </summary>
    ///
    /// <param name="type"> The nonpointer type, such as Short or Double. </typeparam>
    ///
    /// <returns> A VariableType that corresponds to the pointer to a given type. </returns>
    VariableType GetPointerType(VariableType type);

    /// <summary> Gets the default value for a certain type. </summary>
    ///
    /// <typeparam name="ValueType"> The type. </typeparam>
    ///
    /// <returns> The default value for the given type. </returns>
    template <typename ValueType>
    ValueType GetDefaultValue();

    /// <summary> Gets the type-specific add element of the TypedOperator enum. </summary>
    ///
    /// <typeparam name="ValueType"> The type. </typeparam>
    ///
    /// <returns> The add operation that corresponds to the given type. </returns>
    template <typename ValueType>
    TypedOperator GetAddForValueType();

    /// <summary> Gets the type-specific subtract element of the TypedOperator enum. </summary>
    ///
    /// <typeparam name="ValueType"> The type. </typeparam>
    ///
    /// <returns> The subtract operation that corresponds to the given type. </returns>
    template <typename ValueType>
    TypedOperator GetSubtractForValueType();

    /// <summary> Gets the type-specific multiply element of the TypedOperator enum. </summary>
    ///
    /// <typeparam name="ValueType"> The type. </typeparam>
    ///
    /// <returns> The multiply operation that corresponds to the given type. </returns>
    template <typename ValueType>
    TypedOperator GetMultiplyForValueType();

    /// <summary> Gets the type-specific divide element of the TypedOperator enum. </summary>
    ///
    /// <typeparam name="ValueType"> The type. </typeparam>
    ///
    /// <returns> The divide operation that corresponds to the given type. </returns>
    template <typename ValueType>
    TypedOperator GetDivideForValueType();

    /// <summary> Gets the type-specific modulo element of the TypedOperator enum. </summary>
    ///
    /// <typeparam name="ValueType"> The type. </typeparam>
    ///
    /// <returns> The modulo operation that corresponds to the given type. </returns>
    template <typename ValueType>
    TypedOperator GetModForValueType();

    /// <summary> Does the given primitive type have a sign? </summary>
    ///
    /// <param name="type"> The type. </param>
    ///
    /// <returns> true if signed, false if not. </returns>
    bool IsSigned(VariableType type);

    /// <summary> Helper struct for getting the backing value type for a variable </summary>
    template <typename T>
    struct VariableValueType
    {
        using type = T;
        using DestType = type;

        static std::vector<DestType> ToVariableVector(const std::vector<T>& src);
        static std::vector<T> FromVariableVector(const std::vector<DestType>& src);
    };

    template <>
    struct VariableValueType<bool>
    {
        using type = int;
        using DestType = type;

        static std::vector<DestType> ToVariableVector(const std::vector<bool>& src);
        static std::vector<bool> FromVariableVector(const std::vector<DestType>& src);
    };
}
}

#include "../tcc/EmitterTypes.tcc"